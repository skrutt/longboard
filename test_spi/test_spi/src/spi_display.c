/*
 * spi_display.c
 *
 * Created: 2015-10-21 11:51:27
 *  Author: jiut0001
 */ 

#include "spi_display.h"

void ssd1306_clear_buffer() {
	gfx_mono_draw_filled_rect(0, 0, GFX_MONO_LCD_WIDTH, GFX_MONO_LCD_HEIGHT, GFX_PIXEL_CLR);
}