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

/*----------------------
Ska egentligen ligga i gprs_transfer_packages.h
*/
typedef struct {
	uint32_t time;
	float lat;
	float lng;
	float speed;
	uint8_t inclination;
	float g_force;
} log_entry;

typedef struct {
	char date_time[12];		//Används ej
	uint32_t device;
	log_entry entries[255];
} data_log;

/*----------------------*/

typedef struct {
	uint8_t active;
	uint8_t pin;
	uint8_t lastState;
} button;

typedef struct {
	uint8_t speed;
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


#endif /* GLOBALS_H_ */