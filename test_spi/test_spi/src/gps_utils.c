/*
 * gps_utils.c
 *
 * Created: 2015-10-15 15:04:36
 *  Author: jiut0001
 */ 
#include "gps_utils.h"

void gps_utils_raw_data_to_send_buffer(data_log *send_buf) {
	log_entry entry;
	entry.time = gps_data.utc_time;
	entry.lat = gps_data.lat;
	entry.lng = gps_data.lng;
	entry.speed = gps_data.ground_speed;
	entry.inclination = device.inclination;
	entry.g_force = 1.0;
	
	//TODO: add actual value
	entry.upload_interval = 80;				//Upload interval in seconds
	
	entry.cadence = device.cadence;
	
	gprs_buf_push(entry, &gprs_log_buf);
}

//Convert from ddmm.mmmm to decimal coordinates
float gps_utils_coord_to_dec(char* val) {
	float o;

	char minutes[8];
	
	char *dotPointer;
	dotPointer = strchr(val, '.');
	
	char degrees[dotPointer - val];
	
	strncpy(minutes, dotPointer-2, 7);
	strncpy(degrees, val, (dotPointer - val - 2));
	
	o = atof(degrees) + (atof(minutes)/60.0);
	
	return o;
}