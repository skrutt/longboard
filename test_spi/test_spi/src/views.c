/*
 * views.c
 *
 * Created: 2015-10-06 11:54:48
 *  Author: jiut0001
 */ 

#include "menus.h"
#include "views.h"

void display_view(menu_link view) {
	gfx_mono_prev_menu = gfx_mono_active_menu;
	gfx_mono_active_menu = view;
	
	ssd1306_clear_display();
}

//Redraws view and updates data
void refresh_view() {
	switch(gfx_mono_active_menu) {
		case SPEED_VIEW:
			draw_speed_view();
			break;
			
		case NO_GPS_VIEW: 
			if(gps_data.status == 'A') {
				display_view(SPEED_VIEW);
			}
			else {
				draw_no_gps_view();
			}
			break;
	}
}

void draw_no_gps_view() {
	ssd1306_clear_buffer();
	gfx_mono_draw_string("Waiting for",10, 18, &sysfont);
	gfx_mono_draw_string("GPS fix",30, 32, &sysfont);
	ssd1306_write_display();
}

void draw_speed_view() {
	if(gps_data.status != 'A') {
		display_view(NO_GPS_VIEW);
	}
	if(gps_data.ground_speed != device.speed) {
		device.speed = gps_data.ground_speed;
		ssd1306_draw_huge_number(15,1,(uint8_t)(device.speed + 0.5));
	}
}