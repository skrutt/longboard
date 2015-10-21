/*
 * spi_sseg.h
 *
 * Created: 2015-10-16 15:42:34
 *  Author: peol0071
 */ 


#ifndef SPI_SSEG_H_
#define SPI_SSEG_H_


//GPIO pin to use as Slave Select.
#define SLAVE_SELECT_PIN	PIN_PA17
typedef enum {LED_GREEN, LED_RED,LED_BLUE,LED_PURPLE,LED_YELLOW, LED_WHITE} LED_COLORS;


//Struct for led levels
typedef struct {
	uint8_t red_set, green_set, blue_set;
} RGB_LED_t;

//A globally available software device instance struct to store the SPI driver state while it is in use.
struct spi_module spi_master_instance;

//A globally available peripheral slave software device instance struct.
struct spi_slave_inst slave;

 void spi_master_write_done(struct spi_module *const module);
 void configure_spi_master(void);
 void set_disp_led_color(LED_COLORS choice);
 void set_disp_led(RGB_LED_t values);
 
 //Main draw loop
void sseg_update_display(void);
//Set screen brightness
void sseg_set_display_brightness(uint8_t level);

//Update display content
 void set_seg_disp_num(float num);


#endif /* SPI_SSEG_H_ */