/*
 * timer_subsystem.c
 *
 * Created: 2015-10-19 15:00:07
 *  Author: peol0071
 */ 
#include <asf.h>
#include "timer_subsystem.h"

//Set up timer for 1 sek interval
static void configure_tc_logger(void )
{
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	
	//Set clocksource 
	config_tc.clock_source	= GCLK_GENERATOR_1;
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV64;
	
	//config_tc.counter_8_bit.compare_capture_channel[0] = period;
	//config_tc.counter_8_bit.period = period;
	config_tc.counter_8_bit.value = 0x00;
	
	config_tc.pwm_channel[0].enabled = false;
	tc_init(&display_timer_instance, TC3, &config_tc);
	tc_enable(&display_timer_instance);
}

//Set up timer, for fast periods
void configure_tc_bg(uint8_t period)
{
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV64;
	
	config_tc.counter_8_bit.compare_capture_channel[0] = period;
	config_tc.counter_8_bit.period = period;
	config_tc.counter_8_bit.value = 0x00;
	
	config_tc.pwm_channel[0].enabled = false;
	tc_init(&display_timer_instance, TC3, &config_tc);
	tc_enable(&display_timer_instance);
}


//Set up timers:
void configure_tc( )
{

  //configure_tc_logger( );
  configure_tc_bg( 0xff);
}

//Register in callback
void configure_tc_callbacks(tc_callback_t callback_func)
{
	tc_register_callback(	&display_timer_instance,	callback_func, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&display_timer_instance, TC_CALLBACK_OVERFLOW);
}
