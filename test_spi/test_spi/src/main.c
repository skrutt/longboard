
/**
 * \mainpage Application bare main file
 *
 * Bare minimum main file for trip computer
 *
 * Petter Olofsson 2015
 *
 * \par Content
 *
 * Set up platform functions and define current platform in platform.h
 *
 */


#include <asf.h>
#include "spi_sseg.h"
#include "ADC_subsystem.h"
#include "timer_subsystem.h"
#include "button_lib.h"
#include "accelerometer_lib.h"
#include "uart_service.h"
#include "rtc_lib.h"
#include "globals.h"
#include "response_actions.h"
#include "sim808_uart.h"
#include "gprs_transfer_packages.h"

//Include glue file
#include "platform.h"


//Callback for updating display on platform
static void tc_callback_bg_service(struct tc_module *const module_inst)
{
	background_service_platform();
}

//RTC Callback
static void tc_callback_logger_service(void)
{
	run_every_second_platform();
	
	if(gps_logging_enabled) {
		sim808_send_command(CMD_GET_GPS_DATA);
	}
	
	/* Do something on RTC alarm match here */
	//port_pin_toggle_output_level(LED_RTC);
}


int main (void)
{
	//Start and set up system
	system_init();
	delay_init();
	
	//Timer set up
	configure_tc();
	configure_tc_callbacks(tc_callback_bg_service);
	
	//Set up adc
	void (*adc_callbacks[6])(uint16_t) = {update_adxl_gforce_z, update_adxl_gforce_y, update_adxl_gforce_x};
	configure_adc(3, adc_callbacks);
	
	//setup for platform
 	init_platform();
	 
	//Setup SIM808 module
//	sim808_init();
	
	//Wait some for button read and then calibrate adxl
	//init_adxl_calibration(adxl_calibrate_button_platform);
	
	//Start rtc for logging interval
	rtc_lib_configure_soft_alarms();
		
	//Data logging
	rtc_lib_set_soft_alarm_simple(1, tc_callback_logger_service);
	//And uploading
	rtc_lib_set_soft_alarm_simple(10, gprs_send_data_log);
	
	while (true) 
	{
		/* Infinite loop */

		//Update floats from accelerometer
		//recalculate_accelerometer_values();
			
		if(SIM808_buf.available == 1) {
			sim808_parse_response();
		}
			
		//Run platform specifics
		main_platform();
	
		
	}
}
