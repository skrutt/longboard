/*
 * platform.h
 *
 * Created: 2015-10-20 15:26:09
 *  Author: petter
 */ 

//This is just a glue file

#ifndef PLATFORM_H_
#define PLATFORM_H_

#ifdef LONGBOARD
	#include "longboard.h"
	static button_lib_t spi_btn;
	static button_lib_t *adxl_calibrate_button_platform = &spi_btn;
#else
	#include "bike.h"	
	static button_lib_t btn;
	static button_lib_t *adxl_calibrate_button_platform = &btn;
#endif

#endif /* PLATFORM_H_ */