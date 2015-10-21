/*
 * gps_utils.h
 *
 * Created: 2015-10-15 15:04:24
 *  Author: jiut0001
 */ 


#ifndef GPS_UTILS_H_
#define GPS_UTILS_H_

#include <string.h>
#include <stdlib.h>
#include "gprs_transfer_packages.h"
#include "globals.h"

float gps_utils_coord_to_dec(char*);
void gps_utils_raw_data_to_send_buffer(data_log*);



#endif /* GPS_UTILS_H_ */