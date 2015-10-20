/*
 * longboard.h
 *
 * Created: 2015-10-20 13:47:31
 *  Author: petter
 */ 

//To contain all code only relevant for longboard

#ifndef LONGBOARD_H_
#define LONGBOARD_H_

#include <asf.h>
#include <math.h>
#include "spi_sseg.h"
#include "ADC_subsystem.h"
#include "timer_subsystem.h"
#include "button_lib.h"
#include "accelerometer_lib.h"



//longboard defines
button_lib_t spi_btn;

static button_lib_t *adxl_calibrate_button_platform = &spi_btn;

#define LED_SYS				PIN_PA17
#define LED_ADC				PIN_PA18
#define PIEZO_PIN			PIN_PA17

#define LED_usart			PIN_PA16


//Init all longboard specifics
void init_platform(void);

void main_platform(void);

//Run in high speed timer
void background_service_platform(void);

//Ui for calibration
#define		wait_for_x_msg_platform()	set_disp_led_color(LED_GREEN);
#define 	wait_for_y_msg_platform()	set_disp_led_color(LED_YELLOW);
#define 	wait_for_z_msg_platform()	set_disp_led_color(LED_RED);


#endif /* LONGBOARD_H_ */