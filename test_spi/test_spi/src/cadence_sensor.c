/*
 * cadence_sensor.c
 *
 * Created: 2015-10-23 10:13:42
 *  Author: jiut0001
 */ 

#include "cadence_sensor.h"

void cadence_sensor_init() {
	cadence_sensor.cadence = 0;
	cadence_sensor.lastTime = tc_get_count_value(&cadence_timer_instance);
	cadence_sensor.debounce = 300;
	
	//TODO: initiate external interrupt on cadence pin
	cadence_sensor_extint_setup();
	
	delay_ms(500);
	cadence_sensor_update();
}

void cadence_sensor_extint_setup() {
	struct extint_chan_conf config_extint_chan;	
	extint_chan_get_config_defaults(&config_extint_chan);
	
	config_extint_chan.gpio_pin           = PIN_PA07;
	config_extint_chan.gpio_pin_mux		  = PINMUX_PA07A_EIC_EXTINT7;
	config_extint_chan.gpio_pin_pull      = EXTINT_PULL_UP;
	config_extint_chan.detection_criteria = EXTINT_DETECT_FALLING;

	extint_chan_set_config(PA07_EIC_LINE, &config_extint_chan);
	
	// Configure callback
	extint_register_callback(cadence_interrupt_callback, PA07_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
	extint_chan_enable_callback(PA07_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);
}

void cadence_interrupt_callback(void)
{
	cadence_sensor_update();
}

void cadence_sensor_update() {
	uint16_t timerVal = tc_get_count_value(&cadence_timer_instance);
	uint16_t dt = timerVal - cadence_sensor.lastTime;
	if(dt > cadence_sensor.debounce) {
		if(cadence_sensor.cadence == 0) {
			cadence_sensor.cadence = cadence_sensor_calculate_rpm(timerVal);
		}
		else {
			cadence_sensor.cadence = ((cadence_sensor.cadence + cadence_sensor_calculate_rpm(timerVal))/2)+0.5;
		}
		
		cadence_sensor.lastTime = timerVal;			
	}
}

uint8_t cadence_sensor_calculate_rpm(uint16_t timerVal) {
	uint16_t dt = timerVal - cadence_sensor.lastTime;	// Millisecs since last interrupt
	uint16_t cadence = ((60000/dt)+0.5);
	if(cadence > 255) {
		cadence = 255;
	}
	return cadence;
}