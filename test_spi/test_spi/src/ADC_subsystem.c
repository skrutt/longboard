/*
 * ADC_subsystem.c
 *
 * Created: 2015-10-19 14:18:51
 *  Author: peol0071
 */ 
#include "ADC_subsystem.h"

//How many channels we are using
static uint8_t num_channels = 3;
//Callbacks for new value for a channel
void (**callbacks)(uint16_t);

inline float	adc_to_volt(uint16_t val)
{
	//Convert to voltage
	float volt = (float)val/ADC_MAX * VCC;
	
	return  volt;
}
//Call this with number of wanted channels and callbacks for each value
void configure_adc(uint8_t no_channels, void (*register_callbacks[])(uint16_t))
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
	
	//Set up vref to pin 3
	config.mux_position = MUX_PA03B_DAC_VREFP;
	system_pinmux_pin_set_config(PIN_PA03, &config);
	
	//Todo: add switch case here for setting up number of channels: check!
	switch(no_channels)
	{
		//No breaks
		case 5:
			config.mux_position = MUX_PA07B_ADC_AIN7;
			system_pinmux_pin_set_config(PIN_PA04, &config);
		case 4:
			config.mux_position = MUX_PA06B_ADC_AIN6;
			system_pinmux_pin_set_config(PIN_PA06, &config);
		case 3:
			config.mux_position = MUX_PA05B_ADC_AIN5;
			system_pinmux_pin_set_config(PIN_PA05, &config);
		case 2:
			config.mux_position = MUX_PA04B_ADC_AIN4;
			system_pinmux_pin_set_config(PIN_PA04, &config);
		default:
			//No extra pins needed
		break;
	}
	
	//Set up callbacks and num channels
	num_channels = no_channels;
	callbacks = register_callbacks;
	
	//Start reading
	adc_read_buffer_job(&adc_instance, &adc_val, 1);
}

//ADC callback. This is essentially a freerunning adc, just chose how many channels you want
void adc_complete_callback(struct adc_module *const module)
{
	//do something with the average
	static uint8_t curr_channel = 0;
	static const enum adc_positive_input inputs[4] = {ADC_POSITIVE_INPUT_PIN0, ADC_POSITIVE_INPUT_PIN4, ADC_POSITIVE_INPUT_PIN5, ADC_POSITIVE_INPUT_PIN6 };
	
	
// 	//Handle new value
	(callbacks)[curr_channel](adc_val);

	//Restart reading
	curr_channel++;
	//wrap around num channels
	curr_channel %= num_channels;
	
	adc_set_positive_input(&adc_instance, inputs[curr_channel]);
	
	adc_read_buffer_job(&adc_instance , &adc_val, 1);
}
