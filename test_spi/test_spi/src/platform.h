/*
 * platform.h
 *
 * Created: 2015-10-20 15:26:09
 *  Author: petter
 */ 

//This is just a glue file

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "button_lib.h"
#include "cadence_sensor.h"


#define LONGBOARD
#define DEVICE_ID	132

#ifdef LONGBOARD
	#include "longboard.h"
	volatile button_lib_t spi_btn;
	static button_lib_t *adxl_calibrate_button_platform = &spi_btn;
#else
	#include "bike.h"	
	volatile button_lib_t select_btn;
	static button_lib_t *adxl_calibrate_button_platform = &select_btn;
#endif

#endif /* PLATFORM_H_ */