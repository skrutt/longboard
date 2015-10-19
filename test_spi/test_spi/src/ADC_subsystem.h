/*
 * ADC_subsystem.h
 *
 * Created: 2015-10-19 14:19:18
 *  Author: peol0071
 */ 


#ifndef ADC_SUBSYSTEM_H_
#define ADC_SUBSYSTEM_H_


#include <asf.h>

struct adc_module adc_instance;

//uint16_t adc_buffer[4];

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
float	adc_to_volt(uint16_t val);

//Convert from adc to real world
float	adc_to_g_force(uint16_t adc_value, float zero_point, float volt_per_g);

void configure_adc(void);

void adc_complete_callback(struct adc_module *const module);


#endif /* ADC_SUBSYSTEM_H_ */