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
#include "spi_display.h"

typedef struct {
	uint8_t active;
	menu_link view;
} current_view;

void display_view(menu_link);
void display_next_view(void);

//Redraws current view and updates data
void refresh_view(void);

void draw_view(menu_link, uint8_t);

void draw_speed_view(uint8_t);
void draw_cadence_view(uint8_t);
void draw_no_gps_view(uint8_t);

current_view cur_view;


#endif /* VIEWS_H_ */