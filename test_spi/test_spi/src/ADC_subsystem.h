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


// gforce_t	curr_gforce;
 #define ADC_MAX UINT16_MAX
 #define VCC 3.3
 
//Convert from adc to volt
float	adc_to_volt(uint16_t val);

void configure_adc(uint8_t no_channels, void (*callbacks[])(uint16_t));

void adc_complete_callback(struct adc_module *const module);


#endif /* ADC_SUBSYSTEM_H_ */