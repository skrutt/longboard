/*
 * ADC_subsystem.c
 *
 * Created: 2015-10-19 14:18:51
 *  Author: peol0071
 */ 
#include "ADC_subsystem.h"

 float	adc_to_volt(uint16_t val)
{
	//lets see, what to do here

	//Convert to voltage
	float volt = (float)val/ADC_MAX * VCC;
	
	return  volt;
}

//Convert from adc to real world
 float	adc_to_g_force(uint16_t adc_value, float zero_point, float volt_per_g)
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


void configure_adc(void)
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
	
	//Start reading
	adc_read_buffer_job(&adc_instance, &adc_val, 1);
}

void adc_complete_callback(struct adc_module *const module)
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
