/*
 * menu_buttons.h
 *
 * Created: 2015-10-05 14:00:38
 *  Author: jiut0001
 */ 


#ifndef MENU_BUTTONS_H_
#define MENU_BUTTONS_H_

#define BTN_TIMER_MODULE TC3
#define BTN_MENU_SELECT		PIN_PA14
#define BTN_MENU_NAV_DOWN	PIN_PA15

#include <asf.h>
#include "globals.h"

button btn_nav_down;
button btn_nav_select;

extern struct gfx_mono_menu menu_list[];

void menu_buttons_init(void);
void btn_timer_read_callback(struct tc_module *const module_inst);
void btn_timer_config(void);
void btn_timer_config_callbacks(void);


#endif /* MENU_BUTTONS_H_ */