/*
 * accelerometer_lib.c
 *
 * Created: 2015-10-20 12:48:40
 *  Author: petter
 */ 
#include "accelerometer_lib.h"
#include "spi_sseg.h"
#include "button_lib.h"
#include "ADC_subsystem.h"

//Include glue file
#include "platform.h"

//Do the float math reading from raw values
void recalculate_accelerometer_values(void)
{
	accelerometer.scaled_gforce.x	= adc_to_g_force(accelerometer.raw_values.x, accelerometer.x_zero_g_point, accelerometer.x_volt_per_one_g);
	accelerometer.scaled_gforce.y	= adc_to_g_force(accelerometer.raw_values.y, accelerometer.y_zero_g_point, accelerometer.y_volt_per_one_g);
	accelerometer.scaled_gforce.z	= adc_to_g_force(accelerometer.raw_values.z, accelerometer.z_zero_g_point, accelerometer.z_volt_per_one_g);
	//Also add angle
	accelerometer.angle_x = atan( accelerometer.scaled_gforce.y / sqrt( pow(accelerometer.scaled_gforce.x,2) + pow(accelerometer.scaled_gforce.z,2)  ) )* 180.0 / PI;
	accelerometer.angle_y = atan( accelerometer.scaled_gforce.x / sqrt( pow(accelerometer.scaled_gforce.y,2) + pow(accelerometer.scaled_gforce.z,2)  ) )* 180.0 / PI;
	
}

//Generic template for calibrating accelerometer
//TODO: Include support for generic input and output
 void calibrate_accelerometer(ADXL_335_t *calibrate_me, const button_lib_t *wait_button)
{
	//Give shout out
	//printf("\n\rHello and welcome to the generic acccelerometer calibration routine!\n\r");
	
	//Update sseg for headless calibration
	set_seg_disp_num(9999);
	wait_for_button_press(wait_button);
	
	float x_one_g, y_one_g, z_one_g; 
	
	//Get values for first axis, also tell user what to do
	//Using uart for now
	//X
	//printf("We will begin with the X axis, please place the sensor flat with X pointing up.\n\r");	
	
	//set_disp_led_color(LED_GREEN);
	wait_for_x_msg_platform();
	wait_for_button_press(wait_button);

	//Save values for zero points
	calibrate_me->y_zero_g_point = adc_to_volt(calibrate_me->raw_values.y);
	calibrate_me->z_zero_g_point = adc_to_volt(calibrate_me->raw_values.z);
	
	//Save value for one G
	x_one_g = adc_to_volt(calibrate_me->raw_values.x);
	
	
	//Y
	//printf("Now please place the sensor flat with y pointing up.\n\r");
	
	//set_disp_led_color(LED_YELLOW);	
	wait_for_y_msg_platform();
	wait_for_button_press(wait_button);
	
	//Save values for zero points
	calibrate_me->x_zero_g_point = adc_to_volt(calibrate_me->raw_values.x);
	calibrate_me->z_zero_g_point = (adc_to_volt(calibrate_me->raw_values.z) + calibrate_me->z_zero_g_point) / 2;	//Select Z for using average for 0G
	
	//Save value for one G
	y_one_g = adc_to_volt(calibrate_me->raw_values.y);
		
	//Z
	//printf("And lastly please place the sensor flat with Z pointing up.\n\r");
	
//	set_disp_led_color(LED_RED);	
	wait_for_z_msg_platform();
	wait_for_button_press(wait_button);
	
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
	//printf("Sensor calibrated! X 0G: %.3f 1G: %.3f  Y 0G: %.3f 1G: %.3f  Z 0G: %.3f 1G: %.3f \n\r",
	//calibrate_me->x_zero_g_point, calibrate_me->x_volt_per_one_g,
	//calibrate_me->y_zero_g_point, calibrate_me->y_volt_per_one_g, 
	//calibrate_me->z_zero_g_point, calibrate_me->z_volt_per_one_g);
	 
// 	set_disp_led_color(LED_WHITE);
// 	wait_for_button_press(wait_button);
}
//Well, i'll probably let this rest here for now. Although, TODO
 void configure_eeprom(void)
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
 void init_adxl_calibration(button_lib_t *calibrate_button)
{
	
		configure_eeprom();
		//Wait some for button read
		delay_ms(200);

		uint8_t page_data[EEPROM_PAGE_SIZE];
		eeprom_emulator_read_page(0, page_data);
		
		uint8_t is_calibrated = page_data[0];
		ADXL_335_t saved_calibration = *(ADXL_335_t*)&page_data[1];
		
		if ((is_calibrated == 0b10101010) && !button_read_button(calibrate_button))
		{
			accelerometer = saved_calibration;
			//printf("Sensor calibrated with saved values! Press button during start to recalibrate! X 0G: %.3f 1G: %.3f  Y 0G: %.3f 1G: %.3f  Z 0G: %.3f 1G: %.3f \n\r",
			//saved_calibration.x_zero_g_point, saved_calibration.x_volt_per_one_g,
			//saved_calibration.y_zero_g_point, saved_calibration.y_volt_per_one_g,
			//saved_calibration.z_zero_g_point, saved_calibration.z_volt_per_one_g);
			set_disp_led_color(LED_WHITE);
		}else{
			calibrate_accelerometer(&accelerometer, calibrate_button);
		}
}
//Convert from adc to real world
inline float	adc_to_g_force(uint16_t adc_value, float zero_point, float volt_per_g)
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
