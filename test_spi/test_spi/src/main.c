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

//Number of entries in the sample buffer.
#define BUF_LENGTH 20


//GPIO pin to use as Slave Select.
#define SLAVE_SELECT_PIN EXT1_PIN_SPI_SS_0
#define LED_SYS PIN_PA17
#define LED_ADC PIN_PA18

#define LED_usart PIN_PA16
struct usart_module usart_instance;



//A globally available software device instance struct to store the SPI driver state while it is in use.
struct spi_module spi_master_instance;

//A globally available peripheral slave software device instance struct.
struct spi_slave_inst slave;

static void spi_master_write_done(struct spi_module *const module)
{
		spi_select_slave(&spi_master_instance, &slave, false);
}

static void configure_spi_master(void)
{
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
	
	/* Configure and initialize software device instance of peripheral slave */
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = PIN_PA09;
	spi_attach_slave(&slave, &slave_dev_config);
	
	/* Configure, initialize and enable SERCOM SPI module */
	spi_get_config_defaults(&config_spi_master);
	
	config_spi_master.mux_setting = SPI_SIGNAL_MUX_SETTING_E;
	
	/* Configure pad 0 for data in */
	config_spi_master.pinmux_pad0 = PINMUX_UNUSED;
	/* Configure pad 1 as unused */
	config_spi_master.pinmux_pad1 = PINMUX_UNUSED;
	/* Configure pad 2 for data out */
	config_spi_master.pinmux_pad2 = PINMUX_PA10C_SERCOM0_PAD2;
	/* Configure pad 3 for SCK */
	config_spi_master.pinmux_pad3 = PINMUX_PA11C_SERCOM0_PAD3;
	
	spi_init(&spi_master_instance, SERCOM0, &config_spi_master);
	spi_enable(&spi_master_instance);
	
	spi_register_callback(&spi_master_instance, spi_master_write_done, SPI_CALLBACK_BUFFER_TRANSMITTED);
	spi_enable_callback(&spi_master_instance, SPI_CALLBACK_BUFFER_TRANSMITTED);
}

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

uint16_t adc_val[2];

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

//Convert from adc to real world
static float	adc_to_g_force(uint16_t val)
{
	//lets see, what to do here
	//Output from each axis is 0,1 - 2,8V
	
	//Compensate!
	//val = val - (0.1/VCC * ADC_MAX);
	
	//Convert to voltage
	float volt = (float)val/ADC_MAX * VCC;
	
	//convert adc to percentage, center around zero, boost +-0.5 to +-1
	//float percent = ((float)val/(2.8/VCC * ADC_MAX) - 0.5) * 2.0;
	
	//0g is at approx 1/2 VCC
	volt -= VCC/2;
	
	
	// and we have max 3,6 g +/-(typical)
	
	//There is somewhere between 0.3 and 0.306 V per G
	return  volt /(VCC/9.5);
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
			accelerometer.raw_values.z = adc_val;
			//curr_gforce.z	= adc_to_g_force(adc_val[1]);
			break;
		case 1:		//y
			accelerometer.raw_values.y = adc_val;
			//curr_gforce.y	= adc_to_g_force(adc_val[1]);
			break;
		case 2:		//x
			accelerometer.raw_values.x = adc_val;
			//curr_gforce.x	= adc_to_g_force(adc_val[1]);
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
// #define PWM_MODULE EXT1_PWM_MODULE
// #define PWM_OUT_PIN EXT1_PWM_0_PIN
// #define PWM_OUT_MUX EXT1_PWM_0_MUX
//Module to config timer
struct tc_module tc_instance;
//Number on the display
float sseg_num = 0.1;
volatile uint8_t	sseg_brightness = 255;
//Actual data on display
uint8_t sseg_num_num[4];

RGB_LED_t	sseg_leds;
#define RED_LED_BIT 4
#define GREEN_LED_BIT 2
#define BLUE_LED_BIT 8


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

static void configure_tc(uint8_t period);
static void configure_tc_callbacks(void);
//Copy-paste the following callback function code to your user application:
static void tc_callback_update_display(struct tc_module *const module_inst)
{
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
	//config_tc.wave_generation = TC_WAVE;
	config_tc.counter_8_bit.compare_capture_channel[0] = period;
	config_tc.counter_8_bit.period = period;
	config_tc.counter_8_bit.value = 0x00;	
	
	config_tc.pwm_channel[0].enabled = false;
// 	config_tc.pwm_channel[0].pin_out = PWM_OUT_PIN;
// 	config_tc.pwm_channel[0].pin_mux = PWM_OUT_MUX;
	tc_init(&tc_instance, TC3, &config_tc);
	tc_enable(&tc_instance);
}
//Register in callback
static void configure_tc_callbacks(void)
{
	tc_register_callback(	&tc_instance,	tc_callback_update_display, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&tc_instance, TC_CALLBACK_OVERFLOW);
}

//Generic template for calibrating accelerometer
void calibrate_accelerometer(accelerometer calibrate_me)
{
	//Give shout out
	printf("Hello and welcome to the generic acccelerometer calibration routine!\n\r");
	
	//Get values for first axis, also tell user what to do
	//Usint uart for now
	//X
	printf("We will begin with the X axis, please place the sensor flat with X\n\r");	
	
	//Y
	
	//Z
	
	
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
	
	
	
	
	while (true) {
		/* Infinite loop */

			float adc_v = (float)adc_val[1]/UINT16_MAX*3.28 - 1.5;
			
			//float adc_now = (float)adc_val/UINT16_MAX*3.28;
			

			sseg_num+=sseg_num/1000;
			
			set_seg_disp_num(curr_gforce.z);
			
			static uint8_t led_ramp = 4, led_inc = 0;
			
			if (led_inc)
			{
				led_ramp = led_ramp << 1;
			}else{
				led_ramp = led_ramp >> 1 ;
			}
			
			
			if ((led_ramp <= 1) || (led_ramp >= 128))
			{
				led_inc ^= 0xFF;
			}
			
			
			sseg_leds.blue_set = led_ramp;
			sseg_leds.red_set = led_ramp;
			sseg_leds.green_set = 0;
			
			

		
			//printf("woo!! disp at %f\n\r", sseg_num);
			
			port_pin_set_output_level(LED_SYS, true);
			
			printf("adc %.3f X: %.3f Y: %.3f Z: %.3f", adc_v, curr_gforce.x, curr_gforce.y, curr_gforce.z);
			printf("\n\r");
			
			
			port_pin_set_output_level(LED_SYS, false);
	}
}
