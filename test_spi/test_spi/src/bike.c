/*
 * bike.c
 *
 * Created: 2015-10-21 11:59:06
 *  Author: jiut0001
 */ 

#include "platform.h"

#ifndef LONGBOARD


#include "bike.h"

void sim808_fail_to_connect_platform() {
	//TODO: Write message to display
	volatile uint8_t result = 0;
}

void main_platform() {
	sim808_send_command(CMD_GET_GPS_DATA);
	sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
	delay_ms(2000);
}

void init_platform() {
	button_init(&select_btn, PIN_PA14);
	
}

void background_service_platform() {
	button_handler(&select_btn);
}

void dummy() {
	
}

/*
void system_init() {
	delay_init();
	
	//Init uart
	init_SIM808_uart();
	init_sim808_usart_callbacks();
	
	system_interrupt_enable_global();
	
	SIM808_buf.position = 0;
	SIM808_buf.available = 0;
	memset(SIM808_buf.command, 0, sizeof(unsigned char)*COMMAND_BUFFER_SIZE);
	
	sim808_init_commands();
	
	gfx_mono_init();
	
	menu_buttons_init();
	btn_timer_config();
	btn_timer_config_callbacks();
	
	device.speed = 22; //Only for debug

	// The page address to write to
	uint8_t page_address = 0;
	// The column address, or the X pixel.
	uint8_t column_address = 0;

	// Initialize SPI and SSD1306 controller
	ssd1306_init();

	ssd1306_clear_display();
	display_menu(MAIN_MENU);
	
	//Setup GSM key pin
	port_get_config_defaults(&pin_cfg);
	pin_cfg.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(SIM808_RESET_PIN, &pin_cfg);
	port_pin_set_output_level(SIM808_RESET_PIN, true);

	
	ssd1306_clear_buffer();
	gfx_mono_draw_string("Enabling",23, 18, &sysfont);
	gfx_mono_draw_string("GPRS",44, 32, &sysfont);
	ssd1306_write_display();
	
	sim808_init();
	
	ssd1306_clear_buffer();
	gfx_mono_draw_string("Waiting for",10, 18, &sysfont);
	gfx_mono_draw_string("GPS fix",30, 32, &sysfont);
	ssd1306_write_display();
	
	//TODO: Vänta på GPS location fix
	while (gps_data.status != 'A') {
		sim808_send_command(CMD_GET_GPS_DATA);
		sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
		delay_ms(2000);
	}
	
	sim808_send_command(CMD_GET_GPS_DATA);
	sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
	delay_ms(2000);
	sim808_send_command(CMD_GET_GPS_DATA);
	sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
	delay_ms(2000);
	sim808_send_command(CMD_GET_GPS_DATA);
	sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
	
	gprs_send_data_log();
	delay_ms(999000);
	
	gfx_mono_draw_filled_rect(0, 0,
	GFX_MONO_LCD_WIDTH, GFX_MONO_LCD_HEIGHT, GFX_PIXEL_CLR);
	gfx_mono_draw_string("Yeah!",50, 18, &sysfont);
	ssd1306_write_display();
	
	//GPRS GET TEST
	sim808_send_command(CMD_GPRS_GET_REQ);
	sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
	sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
	delay_ms(200000);
}*/


/* INUTI WHILE LOOPEN: 
sim808_send_command(CMD_GET_GPS_DATA);
delay_ms(2000);

if(SIM808_buf.available == 1) {
	sim808_parse_response();
}


if(btn_nav_down.active) {
	btn_nav_down.active = 0;
	gfx_mono_menu_process_key(&menu_list[gfx_mono_active_menu-(VIEW_MAX_INDEX+1)], GFX_MONO_MENU_KEYCODE_DOWN);
	ssd1306_write_display();
}

if(btn_nav_select.active) {
	btn_nav_select.active = 0;
	
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
*/

#endif