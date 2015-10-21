/*
 * longboard.c
 *
 * Created: 2015-10-20 13:49:29
 *  Author: petter
 */ 
#include "longboard.h"

//Init all longboard specifics
void init_platform(void)
{
		button_get_defaults(&spi_btn);
		spi_btn.gpio_pin = PIN_PA10;
		
		//setup outputs
		struct port_config pinconf;
		
		port_get_config_defaults(&pinconf);
		pinconf.direction = PORT_PIN_DIR_OUTPUT;
		
		port_pin_set_config(LED_SYS, &pinconf);		//led_sys
		port_pin_set_config(LED_RTC, &pinconf);	//led_usart
		port_pin_set_config(LED_ADC, &pinconf);		//led_adc
		
		
		//Config spi_sseg
		configure_spi_master();
		
		//Set high brightness for now
		sseg_set_display_brightness(255);
		
}

//Update screen and buttons
void background_service_platform(void)
{
		//Read buttons
		button_handler(&spi_btn);
		
		//Reactivate for spi
		struct system_pinmux_config config;
		system_pinmux_get_config_defaults(&config);
		
		config.mux_position = MUX_PA10C_SERCOM0_PAD2;
		
		system_pinmux_pin_set_config(PIN_PA10, &config);
		
		sseg_update_display();
}

//Main loop for longboard specifics
void main_platform(void)
{
	//what value we are displaying
	static uint8_t disp_val = 0;
	//Soft blink led
	static uint8_t led_ramp = 4, led_inc = 0;

	if (led_inc)
	{
		led_ramp = led_ramp << 1;
		}else{
		led_ramp = led_ramp >> 1;
	}

	if ((led_ramp <= 1) || (led_ramp >= 128))
	{
		led_inc ^= 0xFF;
	}

	if (button_read_button(&spi_btn))
	{
		disp_val++;
		disp_val %= 4;
	}
	RGB_LED_t blink;
	switch(disp_val)
	{
		case 0:
		set_seg_disp_num(accelerometer.scaled_gforce.x);
		blink.blue_set = 0;
		blink.red_set = 0;
		blink.green_set = led_ramp;
		break;
		case 1:
		set_seg_disp_num(accelerometer.scaled_gforce.y);
		blink.blue_set = 0;
		blink.red_set = led_ramp;
		blink.green_set = led_ramp;
		break;
		case 2:
		set_seg_disp_num(accelerometer.scaled_gforce.z);
		blink.blue_set = 0;
		blink.red_set = led_ramp;
		blink.green_set = 0;
		break;
		case 3:
		set_seg_disp_num(accelerometer.angle_x);
		blink.blue_set = led_ramp;
		blink.red_set = 0;
		blink.green_set = 0;
		break;
	
	}
	set_disp_led(blink);

	port_pin_set_output_level(LED_SYS, true);

	printf("X: %.3f Y: %.3f Z: %.3f", accelerometer.scaled_gforce.x, accelerometer.scaled_gforce.y, accelerometer.scaled_gforce.z);
	printf("\n\r");

	port_pin_set_output_level(LED_SYS, false);
}