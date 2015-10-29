/*
 * bike.c
 *
 * Created: 2015-10-21 11:59:06
 *  Author: jiut0001
 */ 

#include "platform.h"
#include "menu_list.h"
#include "views.h"

#ifndef LONGBOARD


#include "bike.h"

void sim808_fail_to_connect_platform() {
	//TODO: Write message to display
	volatile uint8_t result = 0;
}

void main_platform() {
	
	if(button_read_button(&down_btn)) {
		if(is_view(gfx_mono_active_menu)) {
			display_next_view();
		}
		else {
			gfx_mono_menu_process_key(&menu_list[gfx_mono_active_menu-(VIEW_MAX_INDEX+1)], GFX_MONO_MENU_KEYCODE_DOWN);
			ssd1306_write_display();	
		}

	}

	if(button_read_button(&select_btn)) {
		if(is_view(gfx_mono_active_menu)) {
			ssd1306_clear_display();
			display_menu(MAIN_MENU);
		}
		else {
			volatile uint8_t menuChoice = gfx_mono_menu_process_key(&menu_list[gfx_mono_active_menu-(VIEW_MAX_INDEX+1)], GFX_MONO_MENU_KEYCODE_ENTER);
			menu_link menu = menu_list[gfx_mono_active_menu-(VIEW_MAX_INDEX+1)].element_links[menuChoice];
		
			// TODO: Skriv om snyggare
			if(menu == EXIT_MENU) {
				menu_list[gfx_mono_active_menu-(VIEW_MAX_INDEX+1)].current_page = 0;
				menu_list[gfx_mono_active_menu-(VIEW_MAX_INDEX+1)].current_selection = 0;
			
			
				if(is_view(menu_list[gfx_mono_active_menu-(VIEW_MAX_INDEX+1)].parent)) {
					display_view(menu_list[gfx_mono_active_menu-(VIEW_MAX_INDEX+1)].parent);
				}
				else {
					menu = menu_list[gfx_mono_active_menu-(VIEW_MAX_INDEX+1)].parent;
					display_menu(menu);
				}
			
			}
			else {
				display_menu(menu);
			}
		}
	
	}
}

void init_platform() {
	button_init(&select_btn, PIN_PA14);
	button_init(&down_btn, PIN_PA15);
	device.speed = 255;
	
	gfx_mono_init();
	ssd1306_init();
	configure_tc_cadence();
	cadence_sensor_init();
	
	
	// The page address to write to
	uint8_t page_address = 0;
	// The column address, or the X pixel.
	uint8_t column_address = 0;
	
	ssd1306_clear_buffer();
	gfx_mono_draw_string("Enabling",23, 18, &sysfont);
	gfx_mono_draw_string("GPRS",44, 32, &sysfont);
	ssd1306_write_display();
	
	gfx_mono_active_menu = SPEED_VIEW;
	
}

void run_every_second_platform() {
	refresh_view();
}

void background_service_platform() {
	button_handler(&select_btn);
	button_handler(&down_btn);
}

void dummy() {
	
}

#endif