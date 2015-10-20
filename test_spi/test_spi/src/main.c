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
#include "spi_sseg.h"
#include "ADC_subsystem.h"
#include "timer_subsystem.h"
#include "button_lib.h"
#include "accelerometer_lib.h"
#include "uart_service.h"

//Include glue file
#include "platform.h"

//Callback for updating display on longboard
static void tc_callback_update_sseg(struct tc_module *const module_inst)
{
	background_service_platform();
}


int main (void)
{
	system_init();
	delay_init();
	init_uart0();
	
	//Timer set up
	configure_tc(0xff);
	configure_tc_callbacks(tc_callback_update_sseg);
	
	//Set up adc
	void (*adc_callbacks[6])(uint16_t) = {update_adxl_gforce_z, update_adxl_gforce_y, update_adxl_gforce_x};
	configure_adc(3, adc_callbacks);
	
	init_platform();
	
	//Wait some for button read and then calibrate adxl
	init_adxl_calibration(adxl_calibrate_button_platform);
	
	while (true) {
		/* Infinite loop */

			//Update floats from accelerometer
			recalculate_accelerometer_values();
			
			//Run platform specifics
			main_platform();
	}
}
