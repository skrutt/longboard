/*
 * cadence_sensor.c
 *
 * Created: 2015-10-23 10:13:42
 *  Author: jiut0001
 */ 

#include "cadence_sensor.h"

void cadence_sensor_init() {
	cadence_sensor.pin = PIN_PA07;
	cadence_sensor.cadence = 0;
	cadence_sensor.lastTime = tc_get_count_value(&cadence_timer_instance);
	cadence_sensor.debounce = 100;
	
	//TODO: initiate external interrupt on cadence pin
	
	
	delay_ms(500);
	cadence_sensor_update();
}

void cadence_sensor_update() {
		volatile uint16_t timerVal = tc_get_count_value(&cadence_timer_instance);
		if(cadence_sensor.cadence == 0) {
			cadence_sensor.cadence = cadence_sensor_calculate_rpm(timerVal);
		}
		else {
			cadence_sensor.cadence = ((cadence_sensor.cadence + cadence_sensor_calculate_rpm(timerVal))/2)+0.5;
		}
		
		cadence_sensor.lastTime = timerVal;
}

uint8_t cadence_sensor_calculate_rpm(uint16_t timerVal) {
	uint16_t dt = timerVal - cadence_sensor.lastTime;	// Millisecs since last interrupt
	uint16_t cadence = ((60000/dt)+0.5);
	if(cadence > 255) {
		cadence = 255;
	}
	return cadence;
}