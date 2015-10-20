/*
 * accelerometer_lib.h
 *
 * Created: 2015-10-20 12:48:59
 *  Author: petter
 */ 



//Small lib for using ADXL335

#ifndef ACCELEROMETER_LIB_H_
#define ACCELEROMETER_LIB_H_

#include "asf.h"
#include "button_lib.h"
#include "arm_math.h"


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
	
	float angle_x, angle_y;
	
} ADXL_335_t;

//This resides here for now. This could be a pointer in the future
 ADXL_335_t accelerometer;


//Generic template for calibrating accelerometer
//TODO: Include support for generic input and output
 void calibrate_accelerometer(ADXL_335_t *calibrate_me, const button_lib_t *wait_button);

//start eeprom, this could be an internal function
 void configure_eeprom(void);

//Read eeprom, check for calibration and redo if necessary
 void init_adxl_calibration( button_lib_t *calibrate_button);

//Convert from adc to real world
float	adc_to_g_force(uint16_t adc_value, float zero_point, float volt_per_g);

//Do the float math reading from raw values 
 void recalculate_accelerometer_values(void);

//Glue functions
static inline void update_adxl_gforce_x(uint16_t adc_value)
{
	accelerometer.raw_values.x = adc_value;
//	accelerometer.scaled_gforce.x	= adc_to_g_force(adc_value, accelerometer.x_zero_g_point, accelerometer.x_volt_per_one_g);
}
static inline void update_adxl_gforce_y(uint16_t adc_value)
{
	accelerometer.raw_values.y = adc_value;
//	accelerometer.scaled_gforce.y	= adc_to_g_force(adc_value, accelerometer.y_zero_g_point, accelerometer.y_volt_per_one_g);
}
static inline void update_adxl_gforce_z(uint16_t adc_value)
{
	accelerometer.raw_values.z = adc_value;
//	accelerometer.scaled_gforce.z	= adc_to_g_force(adc_value, accelerometer.z_zero_g_point, accelerometer.z_volt_per_one_g);
}



#endif /* ACCELEROMETER_LIB_H_ */