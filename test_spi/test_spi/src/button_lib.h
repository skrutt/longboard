/*
 * button_lib.h
 *
 * Created: 2015-10-19 15:03:09
 *  Author: peol0071
 */ 


#ifndef BUTTON_LIB_H_
#define BUTTON_LIB_H_

#include "asf.h"

//Button on display state
typedef struct {
	volatile bool pressed;		//Is pressed?
	volatile bool read;			//Have we read that it's pressed?
	uint8_t gpio_pin;		//Where is it?
	uint16_t button_debounce;	//Arbitrary number for debounce, depends on update freq
	uint16_t scroll_holdoff;	//Arbitrary number for scrolling list(long press)
	volatile uint16_t scroll_speed;	//Arbitrary number for scrolling freq
	
	bool active_high;	//Is this active high or low?
	
}	button_lib_t;

//Fetch defaults for button
//(Active_low, debounce 20, no scroll, NO PIN so you have to set up
void button_init(button_lib_t * make_me_normal, const uint8_t);
//Wait for button on display to be pressed
 void wait_for_button_press(const button_lib_t * read_me);

//Read and handle buttonpress
 bool button_read_button(button_lib_t * read_me);
//Handler for button
 void button_handler(button_lib_t * btn_to_read);



#endif /* BUTTON_LIB_H_ */