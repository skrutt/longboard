/*
 * longboard.h
 *
 * Created: 2015-10-20 13:47:31
 *  Author: petter
 */ 
#ifndef LONGBOARD_H_
#define LONGBOARD_H_
//To contain all code only relevant for longboard
#include "platform.h"

#ifdef LONGBOARD



#include <asf.h>
//#include <math.h>
#include "spi_sseg.h"
#include "ADC_subsystem.h"
#include "timer_subsystem.h"
#include "button_lib.h"
#include "accelerometer_lib.h"



//longboard defines

//#define LED_SYS				PIN_PA17
//#define LED_ADC				PIN_PA18
//#define PIEZO_PIN			PIN_PA17

//#define LED_RTC				20


//Init all longboard specifics
void init_platform(void);

void main_platform(void);

//Run in high speed timer
void background_service_platform(void);

//Ui for calibration
#define		wait_for_x_msg_platform()	set_disp_led_color(LED_GREEN);
#define 	wait_for_y_msg_platform()	set_disp_led_color(LED_YELLOW);
#define 	wait_for_z_msg_platform()	set_disp_led_color(LED_RED);

//TODO: Cleanup, and msg
void sim808_fail_to_connect_platform(void );


#endif 
#endif /* LONGBOARD_H_ */