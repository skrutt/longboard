/*
 * globals.h
 *
 * Created: 2015-10-05 14:05:59
 *  Author: jiut0001
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <asf.h>

struct port_config pin_cfg;
struct tc_module btn_timer;

typedef struct {
	uint8_t active;
	uint8_t pin;
	uint8_t lastState;
} button;

typedef struct {
	uint8_t speed;
	uint8_t cadence;
	uint8_t lat;
	uint8_t lng;
	uint8_t height;
} position_info;

typedef struct {
	char header[5];
	uint32_t utc_time;
	char status;
	float lat;
	float lng;
	char ns_indicator;
	char ew_indicator;
	float ground_speed;
	float ground_course;
	int date;
	char mode;
	char cheksum[3];
} GPS_GPRMC;

position_info device;

GPS_GPRMC gps_data;
uint8_t gps_logging_enabled;


#endif /* GLOBALS_H_ */