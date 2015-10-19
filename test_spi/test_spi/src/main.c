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
#include "ADC_subsystem.h"
#include "timer_subsystem.h"
#include "button_lib.h"


//Number of entries in the sample buffer.
#define BUF_LENGTH 20
#define PI 3.14159265

button_lib_t spi_btn;



#define LED_SYS				PIN_PA17
#define LED_ADC				PIN_PA18
#define PIEZO_PIN			PIN_PA17

#define LED_usart			PIN_PA16
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


//Timer


//Copy-paste the following callback function code to your user application:
static void tc_callback_update_display(struct tc_module *const module_inst)
{

	
	//Reactivate for spi
	struct system_pinmux_config config;
	system_pinmux_get_config_defaults(&config);
	
	config.mux_position = MUX_PA10C_SERCOM0_PAD2;
	
	system_pinmux_pin_set_config(PIN_PA10, &config);
	
	sseg_update_display();
}

//Generic template for calibrating accelerometer
//TODO: Include support for generic input and output
static void calibrate_accelerometer(ADXL_335_t *calibrate_me)
{
	//Give shout out
	printf("\n\rHello and welcome to the generic acccelerometer calibration routine!\n\r");
	
	//Update sseg for headless calibration
	set_seg_disp_num(9999);
	wait_for_button_press(&spi_btn);
	//	scanf("%s", super_string);	//Wait for user to confirm
	
	float x_one_g, y_one_g, z_one_g; 
	
	//Get values for first axis, also tell user what to do
	//Usint uart for now
	//X
	printf("We will begin with the X axis, please place the sensor flat with X pointing up.\n\r");	
	
	set_disp_led_color(LED_GREEN);
	wait_for_button_press(&spi_btn);
//	scanf("%s", super_string);	//Wait for user to confirm

	//Save values for zero points
	calibrate_me->y_zero_g_point = adc_to_volt(calibrate_me->raw_values.y);
	calibrate_me->z_zero_g_point = adc_to_volt(calibrate_me->raw_values.z);
	
	//Save value for one G
	x_one_g = adc_to_volt(calibrate_me->raw_values.x);
	
	
	//Y
	printf("Now please place the sensor flat with y pointing up.\n\r");
	
	set_disp_led_color(LED_YELLOW);	
	wait_for_button_press(&spi_btn);
	//	scanf("%s", super_string);	//Wait for user to confirm
	
	//Save values for zero points
	calibrate_me->x_zero_g_point = adc_to_volt(calibrate_me->raw_values.x);
	calibrate_me->z_zero_g_point = (adc_to_volt(calibrate_me->raw_values.z) + calibrate_me->z_zero_g_point) / 2;	//Select Z for using average for 0G
	
	//Save value for one G
	y_one_g = adc_to_volt(calibrate_me->raw_values.y);
		
	//Z
	printf("And lastly please place the sensor flat with Z pointing up.\n\r");
	
	set_disp_led_color(LED_RED);	
	wait_for_button_press(&spi_btn);
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
	set_disp_led_color(LED_WHITE);
	wait_for_button_press(&spi_btn);
	//	scanf("%s", super_string);	//Wait for user to confirm
}

static void configure_eeprom(void)
{
    /* Setup EEPROM emulator service */
    enum status_code error_code = eeprom_emulator_init();
    if (error_code == STATUS_ERR_NO_MEMORY) {
        while (true) {
            /* No EEPROM section has been set in the device's fuses */
			set_disp_led_color(LED_RED);
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
		
		if ((is_calibrated == 0b10101010) && !button_read_button(&spi_btn))
		{
			accelerometer = saved_calibration;
			printf("Sensor calibrated with saved values! Press button during start to recalibrate! X 0G: %.3f 1G: %.3f  Y 0G: %.3f 1G: %.3f  Z 0G: %.3f 1G: %.3f \n\r",
			saved_calibration.x_zero_g_point, saved_calibration.x_volt_per_one_g,
			saved_calibration.y_zero_g_point, saved_calibration.y_volt_per_one_g,
			saved_calibration.z_zero_g_point, saved_calibration.z_volt_per_one_g);
			set_disp_led_color(LED_WHITE);
		}else{
			calibrate_accelerometer(&accelerometer);
		}
}

int main (void)
{
	system_init();
	delay_init();
	init_uart0();
	
	button_get_defaults(&spi_btn);
	
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
	configure_tc_callbacks(tc_callback_update_display);
	
	configure_adc();

	configure_spi_master();
	
	configure_eeprom();

	delay_ms(500);
	init_adxl_calibration();
	
	while (true) {
		/* Infinite loop */

			//sseg_num+=sseg_num/1000;
			static uint8_t disp_val = 0;
			
			float angle_accel = atan( accelerometer.scaled_gforce.z / sqrt( pow(accelerometer.scaled_gforce.x,2) + pow(accelerometer.scaled_gforce.y,2)  ) )* 180.0 / PI;
			
			
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
			
			if (button_read_button(&spi_btn))
			{
				disp_val++;
				disp_val %= 4;
			}
			RGB_LED_t blink;
			switch(disp_val)
			{
				case 0:
					set_seg_disp_num(accelerometer.scaled_gforce.x);
					blink.blue_set = 0;
					blink.red_set = 0;
					blink.green_set = led_ramp;
				break;
				case 1:
					set_seg_disp_num(accelerometer.scaled_gforce.y);
					blink.blue_set = 0;
					blink.red_set = led_ramp;
					blink.green_set = led_ramp;
				break;
				case 2:
					set_seg_disp_num(accelerometer.scaled_gforce.z);	
					blink.blue_set = 0;
					blink.red_set = led_ramp;
					blink.green_set = 0;				
				break;
				case 3:
					set_seg_disp_num(angle_accel);
					blink.blue_set = led_ramp;
					blink.red_set = 0;
					blink.green_set = 0;
				break;
				
			}
			set_disp_led(blink);

			port_pin_set_output_level(LED_SYS, true);
			
			printf("X: %.3f Y: %.3f Z: %.3f", accelerometer.scaled_gforce.x, accelerometer.scaled_gforce.y, accelerometer.scaled_gforce.z);
			printf("\n\r");
			
			port_pin_set_output_level(LED_SYS, false);
	}
}
