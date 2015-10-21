/*
 * menus.c
 *
 * Created: 2015-10-05 11:54:08
 *  Author: jiut0001
 */ 

#include "asf.h"
#include "menus.h"

extern struct gfx_mono_menu menu_list[];

void display_menu(menu_link menu) {
	gfx_mono_prev_menu = gfx_mono_active_menu;
	gfx_mono_active_menu = menu;
	gfx_mono_menu_init(&menu_list[menu-(VIEW_MAX_INDEX+1)]);
	ssd1306_write_display();
}

uint8_t is_view(menu_link l) {
	if(l <= VIEW_MAX_INDEX) return 1;
	return 0;
}