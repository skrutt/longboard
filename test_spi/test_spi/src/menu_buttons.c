/*
 * menu_buttons.c
 *
 * Created: 2015-10-05 14:00:05
 *  Author: jiut0001
 */ 

#include "menu_buttons.h"
#include "menus.h"

void menu_buttons_init(void) {
	btn_nav_down.pin = BTN_MENU_NAV_DOWN;
	btn_nav_down.lastState = 1;
	btn_nav_down.active = 0;
	
	btn_nav_select.pin = BTN_MENU_SELECT;
	btn_nav_select.lastState = 1;
	btn_nav_select.active = 0;
	
	port_get_config_defaults(&pin_cfg);
	pin_cfg.direction = PORT_PIN_DIR_INPUT;
	pin_cfg.input_pull = PORT_PIN_PULL_UP;
	
	port_pin_set_config(BTN_MENU_NAV_DOWN, &pin_cfg);
	port_pin_set_config(BTN_MENU_SELECT, &pin_cfg);
}

void btn_timer_read_callback(struct tc_module *const module_inst) {
	volatile bool nav_down_state = port_pin_get_input_level(btn_nav_down.pin);
	volatile bool nav_select_state = port_pin_get_input_level(btn_nav_select.pin);
	
	if(!nav_down_state && btn_nav_down.lastState == true) {
		btn_nav_down.active = 1;
	}
	if(!nav_select_state &&  btn_nav_select.lastState == true) {
		btn_nav_select.active = 1;
	}
	
	btn_nav_down.lastState = nav_down_state;
	btn_nav_select.lastState = nav_select_state;
}

void btn_timer_config(void) {
	struct tc_config config_tc;
	tc_get_config_defaults(&config_tc);
	
	config_tc.counter_size = TC_COUNTER_SIZE_16BIT;
	config_tc.clock_source = GCLK_GENERATOR_0;					// 8 MHZ
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV8;
	
	tc_init(&btn_timer, BTN_TIMER_MODULE, &config_tc);
	tc_enable(&btn_timer);
}

void btn_timer_config_callbacks() {
	tc_register_callback(&btn_timer, btn_timer_read_callback, TC_CALLBACK_OVERFLOW);
	tc_enable_callback(&btn_timer, TC_CALLBACK_OVERFLOW);
}