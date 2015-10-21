/*
 * views.h
 *
 * Created: 2015-10-06 11:54:35
 *  Author: jiut0001
 */ 


#ifndef VIEWS_H_
#define VIEWS_H_

#include <asf.h>
#include "globals.h"

typedef struct {
	uint8_t active;
	menu_link view;
} current_view;

void display_view(menu_link);

//Redraws current view and updates data
void refresh_view(void);

void draw_speed_view(void);

current_view cur_view;


#endif /* VIEWS_H_ */