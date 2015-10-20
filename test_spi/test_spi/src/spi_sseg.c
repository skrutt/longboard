/*
 * spi_sseg.c
 *
 * Created: 2015-10-16 15:41:59
 *  Author: peol0071
 */ 
#include <asf.h>
#include "spi_sseg.h"
#include "timer_subsystem.h"

//Number on the display
float sseg_num;// = 0.1;
volatile uint8_t	sseg_brightness;// = 255;
//Actual data on display
uint8_t sseg_num_num[4];

RGB_LED_t	sseg_leds;

#define RED_LED_BIT 4
#define GREEN_LED_BIT 2
#define BLUE_LED_BIT 8

#define BCM_MIN_STEP 16


 void set_disp_led_color(LED_COLORS choice)
{
	sseg_leds.red_set = 0;
	sseg_leds.green_set = 0;
	sseg_leds.blue_set = 0;
	
	switch(choice)
	{
		case LED_RED:
		sseg_leds.red_set = 255;
		break;
		case LED_GREEN:
		sseg_leds.green_set = 255;
		break;
		case LED_BLUE:
		sseg_leds.blue_set = 255;
		break;
		case LED_PURPLE:
		sseg_leds.red_set = 255;
		sseg_leds.blue_set = 255;
		break;
		case LED_YELLOW:
		sseg_leds.green_set = 255;
		sseg_leds.red_set = 255;
		break;
		case LED_WHITE:
		sseg_leds.green_set = 111;
		sseg_leds.red_set = 111;
		sseg_leds.blue_set = 111;
		break;
	}
}
 void set_disp_led(RGB_LED_t values)
 {
	sseg_leds = values;
 }

 //Convert number to hex
 static uint8_t convert_to_7_seg(uint8_t num)
 {
	static const unsigned char DISPLAY1 [10] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x67};
	 
	 return DISPLAY1[num%10];
 }
 
 void set_seg_disp_num(float num)
 {
	 if (num < 0)
	 {
		 num =-num;
	 }
	 if (num > 999.99)
	 {
		 sseg_num_num[0] = convert_to_7_seg((int)num/1000%10);
		 sseg_num_num[1] = convert_to_7_seg((int)num/100%10);
		 sseg_num_num[2] = convert_to_7_seg((int)num/10%10);
		 sseg_num_num[3] = convert_to_7_seg((int)num%10) + 128;
	 }else 	if (num > 99.99)
	 {
		 sseg_num_num[0] = convert_to_7_seg((int)num/100%10);
		 sseg_num_num[1] = convert_to_7_seg((int)num/10%10);
		 sseg_num_num[2] = convert_to_7_seg((int)num%10) + 128;
		 sseg_num_num[3] = convert_to_7_seg((num - (int)num)*10);
	 }else if (num > 9.999)
	 {
		 sseg_num_num[0] = convert_to_7_seg((int)num/10%10);
		 sseg_num_num[1] = convert_to_7_seg((int)num%10) + 128;
		 sseg_num_num[2] = convert_to_7_seg((num - (int)num)*10);
		 sseg_num_num[3] = convert_to_7_seg((num * 10 - (int)(num* 10))*10);
	 }else
	 {
		 sseg_num_num[0] = convert_to_7_seg((int)num) + 128;
		 sseg_num_num[1] = convert_to_7_seg((num - (int)num)*10);
		 sseg_num_num[2] = convert_to_7_seg((num * 10 - (int)(num* 10))*10);
		 sseg_num_num[3] = convert_to_7_seg((num * 100 - (int)(num* 100))*10);
	 }
	 //Failsafe for brightness, always turn display on at least the lowest brightness
	 if (sseg_brightness < BCM_MIN_STEP)
	 {
		 sseg_brightness = BCM_MIN_STEP;
	 }

 }

//Set screen brightness
void sseg_set_display_brightness(uint8_t level)
{
	sseg_brightness = level;
}

//Redraw display and handle leds
void sseg_update_display(void)
{
//	port_pin_toggle_output_level(LED_usart);

	static uint8_t bcm_cycle = 16;
	static uint8_t active_num = 0;
	static uint8_t buf[2];

	active_num ++;
	if (active_num >= 4)
	{
		active_num = 0;
		//all segs done, go to next level
		bcm_cycle = bcm_cycle << 1;
		//reset
		if (bcm_cycle == 0)
		{
			bcm_cycle = BCM_MIN_STEP;
		}
		tc_set_top_value(&display_timer_instance, bcm_cycle);
	}

	// 	//Get the proper number
	buf[0] = (sseg_num_num[active_num] & 0xf0);	//high 7seg nibble + rgb

	//Light led for each bcm level
	if(!(sseg_leds.red_set & bcm_cycle))
	{
		buf[0] += RED_LED_BIT;
	}
	if(!(sseg_leds.green_set & bcm_cycle))
	{
		buf[0] += GREEN_LED_BIT;
	}
	if(!(sseg_leds.blue_set & bcm_cycle))
	{
		buf[0] += BLUE_LED_BIT;
	}

	buf[1] = (sseg_num_num[active_num] & 0x0f) ;	//U1!!!!!!!!!!!!!!!!!!	//low 7seg nibble	&& segment select is at high nibble

	spi_select_slave(&spi_master_instance, &slave, true);

	if(sseg_brightness & bcm_cycle)
	{
		buf[1] +=  (~(1 << (7 - active_num)) & 0xf0);
	
	}else{
		buf[1] += 0xF0;
	}

	spi_write_buffer_job(&spi_master_instance, buf, 2);

}

 void spi_master_write_done(struct spi_module *const module)
{
	spi_select_slave(&spi_master_instance, &slave, false);
	//configure datapin for reading button
	//here...
	
	//Datapin is pa10
	struct system_pinmux_config config;
	system_pinmux_get_config_defaults(&config);
	
	config.direction   = SYSTEM_PINMUX_PIN_DIR_INPUT;
	config.input_pull   = SYSTEM_PINMUX_PIN_PULL_UP;
	config.mux_position = SYSTEM_PINMUX_GPIO;
	
	system_pinmux_pin_set_config(PIN_PA10, &config);	//Todo, set this to read from struct
}

 void configure_spi_master(void)
{
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
	
	/* Configure and initialize software device instance of peripheral slave */
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = SLAVE_SELECT_PIN;
	spi_attach_slave(&slave, &slave_dev_config);
	
	/* Configure, initialize and enable SERCOM SPI module */
	spi_get_config_defaults(&config_spi_master);
	
	config_spi_master.mux_setting = SPI_SIGNAL_MUX_SETTING_E;
	
	/* Configure pad 0 for data in */
	config_spi_master.pinmux_pad0 = PINMUX_UNUSED;
	/* Configure pad 1 as unused */
	config_spi_master.pinmux_pad1 = PINMUX_UNUSED;
	/* Configure pad 2 for data out */
	config_spi_master.pinmux_pad2 = PINMUX_PA10C_SERCOM0_PAD2;
	/* Configure pad 3 for SCK */
	config_spi_master.pinmux_pad3 = PINMUX_PA11C_SERCOM0_PAD3;
	
	spi_init(&spi_master_instance, SERCOM0, &config_spi_master);
	spi_enable(&spi_master_instance);
	
	spi_register_callback(&spi_master_instance, spi_master_write_done, SPI_CALLBACK_BUFFER_TRANSMITTED);
	spi_enable_callback(&spi_master_instance, SPI_CALLBACK_BUFFER_TRANSMITTED);
}
