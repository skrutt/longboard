/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <math.h>
#include "spi_sseg.h"


//Number of entries in the sample buffer.
#define BUF_LENGTH 20
#define PI 3.14159265


#define LED_SYS				PIN_PA17
#define LED_ADC				PIN_PA18
#define PIEZO_PIN			PIN_PA17

#define LED_usart PIN_PA16
struct usart_module usart_instance;



static void init_uart0(void)
{
	struct usart_config uart_settings;
	usart_get_config_defaults(&uart_settings);
	
	//Set clock to 8M
	uart_settings.generator_source = GCLK_GENERATOR_3;
	
	uart_settings.mux_setting = USART_RX_1_TX_2_XCK_3;
	uart_settings.pinmux_pad0 = PINMUX_PA24C_SERCOM3_PAD2; // Tx
	uart_settings.pinmux_pad1 = PINMUX_PA23C_SERCOM3_PAD1; // Rx
	uart_settings.pinmux_pad2 = PINMUX_UNUSED;
	uart_settings.pinmux_pad3 = PINMUX_UNUSED;
	uart_settings.baudrate = 11500;
	while (usart_init(&usart_instance, SERCOM3, &uart_settings) != STATUS_OK){}
	
	stdio_serial_init(&usart_instance, SERCOM3, &uart_settings);
	usart_enable(&usart_instance);
}

struct adc_module adc_instance;

uint16_t adc_buffer[4];

uint16_t adc_val;

//Struct for gforce in space
typedef struct
{
	float x,y,z;
} gforce_t;

//Struct for raw values from accelerometer
typedef struct
{
	uint16_t x,y,z;
} accelerometer_raw_t;

typedef struct  
{
	 accelerometer_raw_t raw_values;		//values read from adc
	 gforce_t	scaled_gforce;					//ready to use gforce
	 float x_zero_g_point, y_zero_g_point, z_zero_g_point;	//calibration value, idle point
	 float x_volt_per_one_g, y_volt_per_one_g, z_volt_per_one_g; //Calibration value, one g gives this response on output
	 
} ADXL_335_t;



 ADXL_335_t accelerometer;
// gforce_t	curr_gforce;
 #define ADC_MAX UINT16_MAX
 #define VCC 3.3
//Convert from adc to volt
static float	adc_to_volt(uint16_t val)
{
	//lets see, what to do here

	//Convert to voltage
	float volt = (float)val/ADC_MAX * VCC;
	
	return  volt;
}

//Convert from adc to real world
static float	adc_to_g_force(uint16_t adc_value, float zero_point, float volt_per_g)
{
	//lets see, what to do here
	
	//Convert to voltage
	float volt = adc_to_volt(adc_value);
	
	//zero reading
	volt -= zero_point;
	
	
	//There is somewhere between 0.3 and 0.306 V per G
	//Divide out reading
	return  volt /volt_per_g;
}

static inline void update_adxl_gforce_x(uint16_t adc_value)
{
	accelerometer.raw_values.x = adc_value;
	accelerometer.scaled_gforce.x	= adc_to_g_force(adc_value, accelerometer.x_zero_g_point, accelerometer.x_volt_per_one_g);
}
static inline void update_adxl_gforce_y(uint16_t adc_value)
{
	accelerometer.raw_values.y = adc_value;
	accelerometer.scaled_gforce.y	= adc_to_g_force(adc_value, accelerometer.y_zero_g_point, accelerometer.y_volt_per_one_g);
}
static inline void update_adxl_gforce_z(uint16_t adc_value)
{
	accelerometer.raw_values.z = adc_value;
	accelerometer.scaled_gforce.z	= adc_to_g_force(adc_value, accelerometer.z_zero_g_point, accelerometer.z_volt_per_one_g);
}


static void adc_complete_callback(struct adc_module *const module)
{
	//compute the average
// 	uint32_t avg = 0;
// 	for(uint8_t i=0;i<4;i++)
// 	{
// 		avg += adc_buffer[i];
// 	}
// 	
// 	avg >>= 2;
// 	
// 	adc_val = avg;
	
	//do something with the average
	static const uint8_t num_channels = 3;
	static uint8_t curr_channel = 0;
	static const enum adc_positive_input inputs[3] = {ADC_POSITIVE_INPUT_PIN0, ADC_POSITIVE_INPUT_PIN4, ADC_POSITIVE_INPUT_PIN5 };
		
	
	//Handle new value
	switch(curr_channel)
	{
		case 0:		//Z
			update_adxl_gforce_z(adc_val);
			break;
		case 1:		//y
			update_adxl_gforce_y(adc_val);
			break;
		case 2:		//x
			update_adxl_gforce_x(adc_val);
			break;
		//Add more channels here
		default:
		break;
		//oops
	}
	
	
	//Restart reading
	curr_channel++;
	if (curr_channel == num_channels)
	{
		curr_channel = 0;
	}
	
	adc_set_positive_input(&adc_instance, inputs[curr_channel]);
	
	adc_read_buffer_job(&adc_instance , &adc_val, 1);
	
	
}

static void configure_adc(void)
{
	struct adc_config config_adc;
	adc_get_config_defaults(&config_adc);
	
	config_adc.gain_factor = ADC_GAIN_FACTOR_1X;
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV512;
	//Select reference
	config_adc.reference = ADC_REFCTRL_REFSEL_AREFA;
	config_adc.positive_input = ADC_POSITIVE_INPUT_PIN0;
	config_adc.resolution = ADC_RESOLUTION_16BIT;
	
	adc_init(&adc_instance, ADC, &config_adc);
	
	adc_enable(&adc_instance);
	
	adc_register_callback(&adc_instance, adc_complete_callback, ADC_CALLBACK_READ_BUFFER);
	adc_enable_callback(&adc_instance, ADC_CALLBACK_READ_BUFFER);
	
	/* Configure the rest of the analog pins */
	struct system_pinmux_config config;
	system_pinmux_get_config_defaults(&config);
	
	/* Analog functions are all on MUX setting B */
	config.input_pull   = SYSTEM_PINMUX_PIN_PULL_NONE;
	config.mux_position = MUX_PA04B_ADC_AIN4;
	
	system_pinmux_pin_set_config(PIN_PA04, &config);
	config.mux_position = MUX_PA05B_ADC_AIN5;
	system_pinmux_pin_set_config(PIN_PA05, &config);
}

//Convert number to hex
static uint8_t convert_to_7_seg(uint8_t num)
{
	const unsigned char DISPLAY1 [10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67};
	
	return DISPLAY1[num%10];
	
}
//Struct for led levels
typedef struct {
	uint8_t red_set, green_set, blue_set;
} RGB_LED_t;

//Timer

//Module to config timer
struct tc_module tc_instance;
//Number on the display
float sseg_num = 0.1;
volatile uint8_t	sseg_brightness = 255;
//Actual data on display
uint8_t sseg_num_num[4];
//Button on display state
struct {
	volatile bool pressed;
	volatile bool read;
	
	}	spi_button;
volatile uint16_t spi_button_debounce = 0;

RGB_LED_t	sseg_leds;
#define RED_LED_BIT 4
#define GREEN_LED_BIT 2
#define BLUE_LED_BIT 8
typedef enum {LED_GREEN, LED_RED,LED_BLUE,LED_PURPLE,LED_YELLOW, LED_WHITE} LED_COLORS;


static void set_seg_disp_num(float num)
{
	if (num < 0)
	{
		num =-num;
	}
	if (num > 999.99)
	{
		sseg_num_num[0] = convert_to_7_seg((int)num/1000%10);
		sseg_num_num[1] = convert_to_7_seg((int)num/100%10);
		sseg_num_num[2] = convert_to_7_seg((int)num/10%10);
		sseg_num_num[3] = convert_to_7_seg((int)num%10) + 128;
	}else 	if (num > 99.99)
	{
		sseg_num_num[0] = convert_to_7_seg((int)num/100%10);
		sseg_num_num[1] = convert_to_7_seg((int)num/10%10);
		sseg_num_num[2] = convert_to_7_seg((int)num%10) + 128;
		sseg_num_num[3] = convert_to_7_seg((num - (int)num)*10);
	}else if (num > 9.999)
	{
		sseg_num_num[0] = convert_to_7_seg((int)num/10%10);
		sseg_num_num[1] = convert_to_7_seg((int)num%10) + 128;
		sseg_num_num[2] = convert_to_7_seg((num - (int)num)*10);
		sseg_num_num[3] = convert_to_7_seg((num * 10 - (int)(num* 10))*10);
	}else
	{
		sseg_num_num[0] = convert_to_7_seg((int)num) + 128;
		sseg_num_num[1] = convert_to_7_seg((num - (int)num)*10);
		sseg_num_num[2] = convert_to_7_seg((num * 10 - (int)(num* 10))*10);
		sseg_num_num[3] = convert_to_7_seg((num * 100 - (int)(num* 100))*10);
	}

}
static void set_disp_led(LED_COLORS choice)
{
	sseg_leds.red_set = 0;
	sseg_leds.green_set = 0;
	sseg_leds.blue_set = 0;
	
	switch(choice)
	{
		case LED_RED:
			sseg_leds.red_set = 255;
		break;
		case LED_GREEN:
			sseg_leds.green_set = 255;
		break;
		case LED_BLUE:
			sseg_leds.blue_set = 255;
		break;
		case LED_PURPLE:
			sseg_leds.red_set = 255;
			sseg_leds.blue_set = 255;			
		break;
		case LED_YELLOW:
			sseg_leds.green_set = 255;
			sseg_leds.red_set = 255;
		break;
		case LED_WHITE:
			sseg_leds.green_set = 111;
			sseg_leds.red_set = 111;
			sseg_leds.blue_set = 111;
		break;
	}
}

static void configure_tc(uint8_t period);
static void configure_tc_callbacks(void);
//Read and handle buttonpress
static inline bool spi_read_button(void)
{
	if (spi_button.pressed && !spi_button.read)
	{
		spi_button.read = true;
		return true;
	}
	return false;
}

//Copy-paste the following callback function code to your user application:
static void tc_callback_update_display(struct tc_module *const module_inst)
{
	//Read button! and then activate spi again
	//Read.?
	if (!port_pin_get_input_level(PIN_PA10))
	{
		spi_button_debounce++;
		if (spi_button_debounce >= 100 && !spi_button.pressed)
		{
			spi_button.pressed = true;
			spi_button.read = false;
		}
		
	}else{
		if (spi_button.read)
		{
			spi_button.pressed = false;
		}
		spi_button_debounce = 0;	
	}
	
	//Reactivate for spi
	struct system_pinmux_config config;
	system_pinmux_get_config_defaults(&config);
	
	//config->direction   = SYSTEM_PINMUX_PIN_DIR_INPUT;
	//config.input_pull   = SYSTEM_PINMUX_PIN_PULL_NONE;
	config.mux_position = MUX_PA10C_SERCOM0_PAD2;
	
	system_pinmux_pin_set_config(PIN_PA10, &config);
	
	port_pin_toggle_output_level(LED_usart);
	
	static uint8_t bcm_cycle = 16;
	static uint8_t active_num = 0;
	static uint8_t buf[2];
	
	active_num ++;
	if (active_num >= 4)
	{
		active_num = 0;
		//all segs done, go to next level
		bcm_cycle = bcm_cycle << 1;
		//reset
		if (bcm_cycle == 0)
		{
			bcm_cycle = 16;
		}
		tc_set_top_value(&tc_instance, bcm_cycle);
	}
	
	// 	//Get the proper number
 	buf[0] = (sseg_num_num[active_num] & 0xf0);	//high 7seg nibble + rgb
	
	//Light led for each bcm level
	if(!(sseg_leds.red_set & bcm_cycle))
	{
		buf[0] += RED_LED_BIT;
	}
	if(!(sseg_leds.green_set & bcm_cycle))
	{
		buf[0] += GREEN_LED_BIT;
	}
	if(!(sseg_leds.blue_set & bcm_cycle))
	{
		buf[0] += BLUE_LED_BIT;
	}

 	buf[1] = (sseg_num_num[active_num] & 0x0f) ;	//U1!!!!!!!!!!!!!!!!!!	//low 7seg nibble	&& segment select is at high nibble
	 
 	spi_select_slave(&spi_master_instance, &slave, true);

 	if(sseg_brightness & bcm_cycle)
 	{
 		buf[1] +=  (~(1 << (7 - active_num)) & 0xf0);
		 
 	}else{
 		buf[1] += 0xF0;
 	}
	
	spi_write_buffer_job(&spi_master_instance, buf, 2);
	
}
//Copy-paste the following setup code to your user application:
static void configure_tc(uint8_t period)
{
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV64;
	
	config_tc.counter_8_bit.compare_capture_channel[0] = period;
	config_tc.counter_8_bit.period = period;
	config_tc.counter_8_bit.value = 0x00;	
	
	config_tc.pwm_channel[0].enabled = false;
	tc_init(&tc_instance, TC3, &config_tc);
	tc_enable(&tc_instance);
}
//Register in callback
static void configure_tc_callbacks(void)
{
	tc_register_callback(	&tc_instance,	tc_callback_update_display, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
}

//Wait for button on display to be pressed
static inline void wait_for_button_press(void)
{
	while (!spi_read_button());
}
//Generic template for calibrating accelerometer
//TODO: Include support for generic input and output
static void calibrate_accelerometer(ADXL_335_t *calibrate_me)
{
	//Give shout out
	printf("\n\rHello and welcome to the generic acccelerometer calibration routine!\n\r");
	
	//Update sseg for headless calibration
	set_seg_disp_num(9999);
	wait_for_button_press();
	//	scanf("%s", super_string);	//Wait for user to confirm
	
	float x_one_g, y_one_g, z_one_g; 
	
	//Get values for first axis, also tell user what to do
	//Usint uart for now
	//X
	printf("We will begin with the X axis, please place the sensor flat with X pointing up.\n\r");	
	
	set_disp_led(LED_GREEN);
	wait_for_button_press();
//	scanf("%s", super_string);	//Wait for user to confirm

	//Save values for zero points
	calibrate_me->y_zero_g_point = adc_to_volt(calibrate_me->raw_values.y);
	calibrate_me->z_zero_g_point = adc_to_volt(calibrate_me->raw_values.z);
	
	//Save value for one G
	x_one_g = adc_to_volt(calibrate_me->raw_values.x);
	
	
	//Y
	printf("Now please place the sensor flat with y pointing up.\n\r");
	
	set_disp_led(LED_YELLOW);	
	wait_for_button_press();
	//	scanf("%s", super_string);	//Wait for user to confirm
	
	//Save values for zero points
	calibrate_me->x_zero_g_point = adc_to_volt(calibrate_me->raw_values.x);
	calibrate_me->z_zero_g_point = (adc_to_volt(calibrate_me->raw_values.z) + calibrate_me->z_zero_g_point) / 2;	//Select Z for using average for 0G
	
	//Save value for one G
	y_one_g = adc_to_volt(calibrate_me->raw_values.y);
		
	//Z
	printf("And lastly please place the sensor flat with Z pointing up.\n\r");
	
	set_disp_led(LED_RED);	
	wait_for_button_press();
	//	scanf("%s", super_string);	//Wait for user to confirm
	
	//Save values for zero points
	calibrate_me->x_zero_g_point = (adc_to_volt(calibrate_me->raw_values.x) + calibrate_me->x_zero_g_point) / 2;
	calibrate_me->y_zero_g_point = (adc_to_volt(calibrate_me->raw_values.y) + calibrate_me->y_zero_g_point) / 2;
	
	//Save value for one G
	z_one_g = adc_to_volt(calibrate_me->raw_values.z);
	
	//Now, calculate and save calibration
	calibrate_me->x_volt_per_one_g = x_one_g - calibrate_me->x_zero_g_point;
	calibrate_me->y_volt_per_one_g = y_one_g - calibrate_me->y_zero_g_point;
	calibrate_me->z_volt_per_one_g = z_one_g - calibrate_me->z_zero_g_point;
	
	
	//Save data into eeprom
	uint8_t page_data[EEPROM_PAGE_SIZE];
	
	//Set calibration flag
	uint8_t is_calibrated = 0b10101010;
	page_data[0] = is_calibrated;
	
	*(ADXL_335_t*)&page_data[1] = *calibrate_me;
			
	eeprom_emulator_write_page(0, page_data);
	eeprom_emulator_commit_page_buffer();
	
	//Done!
	printf("Sensor calibrated! X 0G: %.3f 1G: %.3f  Y 0G: %.3f 1G: %.3f  Z 0G: %.3f 1G: %.3f \n\r",
	 calibrate_me->x_zero_g_point, calibrate_me->x_volt_per_one_g,
	 calibrate_me->y_zero_g_point, calibrate_me->y_volt_per_one_g, 
	 calibrate_me->z_zero_g_point, calibrate_me->z_volt_per_one_g);
	set_disp_led(LED_WHITE);
	wait_for_button_press();
	//	scanf("%s", super_string);	//Wait for user to confirm
}

static void configure_eeprom(void)
{
    /* Setup EEPROM emulator service */
    enum status_code error_code = eeprom_emulator_init();
    if (error_code == STATUS_ERR_NO_MEMORY) {
        while (true) {
            /* No EEPROM section has been set in the device's fuses */
			set_disp_led(LED_RED);
        }
    }
    else if (error_code != STATUS_OK) {
        /* Erase the emulated EEPROM memory (assume it is unformatted or
         * irrecoverably corrupt) */
        eeprom_emulator_erase_memory();
        eeprom_emulator_init();
    }
}
//Read eeprom, check for calibration and redo if necessary
static void init_adxl_calibration(void)
{
		configure_eeprom();

		uint8_t page_data[EEPROM_PAGE_SIZE];
		eeprom_emulator_read_page(0, page_data);
		
		uint8_t is_calibrated = page_data[0];
		ADXL_335_t saved_calibration = *(ADXL_335_t*)&page_data[1];
		
		if ((is_calibrated == 0b10101010) && !spi_read_button())
		{
			accelerometer = saved_calibration;
			printf("Sensor calibrated with saved values! Press button during start to recalibrate! X 0G: %.3f 1G: %.3f  Y 0G: %.3f 1G: %.3f  Z 0G: %.3f 1G: %.3f \n\r",
			saved_calibration.x_zero_g_point, saved_calibration.x_volt_per_one_g,
			saved_calibration.y_zero_g_point, saved_calibration.y_volt_per_one_g,
			saved_calibration.z_zero_g_point, saved_calibration.z_volt_per_one_g);
			set_disp_led(LED_WHITE);
		}else{
			calibrate_accelerometer(&accelerometer);
		}
}
int main (void)
{
	system_init();
	delay_init();
	init_uart0();
	
	struct port_config pinconf;
	
	//led_sys
	port_get_config_defaults(&pinconf);
	pinconf.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_SYS, &pinconf);
	
	//led_usart
	port_pin_set_config(LED_usart, &pinconf);
	//led_adc
	port_pin_set_config(LED_ADC, &pinconf);
	
	/* Configure analog pins */
	struct system_pinmux_config config;
	system_pinmux_get_config_defaults(&config);
	
	/* Analog functions are all on MUX setting B */
	config.input_pull   = SYSTEM_PINMUX_PIN_PULL_NONE;
	config.mux_position = MUX_PA03B_DAC_VREFP;
	
	system_pinmux_pin_set_config(PIN_PA03, &config);

//Timer set up
	configure_tc(0xff);
	configure_tc_callbacks();
	
	configure_adc();

	configure_spi_master();
	
	//Start reading
	adc_read_buffer_job(&adc_instance,adc_buffer,4);
	
	configure_eeprom();

	delay_ms(500);
	init_adxl_calibration();
	
	while (true) {
		/* Infinite loop */

			sseg_num+=sseg_num/1000;
			static uint8_t disp_val = 0;
			
			float angle_accel = atan( accelerometer.scaled_gforce.y / sqrt( pow(accelerometer.scaled_gforce.x,2) + pow(accelerometer.scaled_gforce.z,2)  ) )* 180.0 / PI;
			
			
						//Soft blink led
			static uint8_t led_ramp = 4, led_inc = 0;
			
			if (led_inc)
			{
				led_ramp = led_ramp << 1;
			}else{
				led_ramp = led_ramp >> 1;
			}
			
			if ((led_ramp <= 1) || (led_ramp >= 128))
			{
				led_inc ^= 0xFF;
			}
			
			if (spi_read_button())
			{
				disp_val++;
				disp_val %= 4;
			}
			switch(disp_val)
			{
				case 0:
					set_seg_disp_num(accelerometer.scaled_gforce.x);
					sseg_leds.blue_set = 0;
					sseg_leds.red_set = 0;
					sseg_leds.green_set = led_ramp;
				break;
				case 1:
					set_seg_disp_num(accelerometer.scaled_gforce.y);
					sseg_leds.blue_set = 0;
					sseg_leds.red_set = led_ramp;
					sseg_leds.green_set = led_ramp;
				break;
				case 2:
					set_seg_disp_num(accelerometer.scaled_gforce.z);	
					sseg_leds.blue_set = 0;
					sseg_leds.red_set = led_ramp;
					sseg_leds.green_set = 0;				
				break;
				case 3:
					set_seg_disp_num(angle_accel);
					sseg_leds.blue_set = led_ramp;
					sseg_leds.red_set = 0;
					sseg_leds.green_set = 0;
				break;
				
			}

			port_pin_set_output_level(LED_SYS, true);
			
			printf("X: %.3f Y: %.3f Z: %.3f", accelerometer.scaled_gforce.x, accelerometer.scaled_gforce.y, accelerometer.scaled_gforce.z);
			printf("\n\r");
			
			port_pin_set_output_level(LED_SYS, false);
	}
}
