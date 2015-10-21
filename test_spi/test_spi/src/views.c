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
	
	if(view == SPEED_VIEW) {
		draw_speed_view();
	}
}

//Redraws current view and updates data
void refresh_view() {
	
}

void draw_speed_view() {
	ssd1306_clear_display();
	ssd1306_draw_huge_number(15,1,device.speed);
}