/*
 * menus.h
 *
 * Created: 2015-10-02 17:35:31
 *  Author: Jimmy
 */ 


#ifndef MENUS_H_
#define MENUS_H_

#include <asf.h>
#include "menu_link.h"

menu_link gfx_mono_active_menu;
menu_link gfx_mono_prev_menu;

void display_menu(menu_link);
uint8_t is_view(menu_link);


#endif /* MENUS_H_ */