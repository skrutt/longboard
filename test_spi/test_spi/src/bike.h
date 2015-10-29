/*
 * bike.h
 *
 * Created: 2015-10-21 11:58:48
 *  Author: jiut0001
 */ 


#ifndef BIKE_H_
#define BIKE_H_

#include <asf.h>
#include "button_lib.h"
#include "sim808_uart.h"
#include "menus.h"

void dummy(void);

void wait_for_x_msg_platform(void);
void wait_for_y_msg_platform(void);
void wait_for_z_msg_platform(void);
void display_adc_calibration_msg(unsigned char);

void before_sim_init_platform(void);

void system_init(void);
void sim808_fail_to_connect_platform(void);
void before_main_loop_platform(void);
void main_platform(void);
void init_platform(void);
void background_service_platform(void);
void run_every_second_platform(void);

//DEBUG ONLY:
uint8_t gps_counter;

button_lib_t down_btn;


#endif /* BIKE_H_ */