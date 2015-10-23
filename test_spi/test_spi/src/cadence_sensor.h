/*
 * cadence_sensor.h
 *
 * Created: 2015-10-23 10:13:30
 *  Author: jiut0001
 */ 


#ifndef CADENCE_SENSOR_H_
#define CADENCE_SENSOR_H_

#include <asf.h>
#include "timer_subsystem.h"

typedef struct {
	uint8_t pin;
	uint8_t cadence;  //RPM
	uint8_t debounce; //In milliseconds
	uint16_t lastTime;
} cadence_module;

cadence_module cadence_sensor;

void cadence_sensor_init(void);
void cadence_sensor_update(void);
uint8_t cadence_sensor_calculate_rpm(uint16_t);



#endif /* CADENCE_SENSOR_H_ */