/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>

//Number of entries in the sample buffer.
#define BUF_LENGTH 20

//A sample buffer to send via SPI.
static uint8_t buffer[BUF_LENGTH] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
	0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13
};

//GPIO pin to use as Slave Select.
#define SLAVE_SELECT_PIN EXT1_PIN_SPI_SS_0
#define LED_SYS PIN_PA02

struct usart_module usart_instance;
#define LED_usart PIN_PA16


//A globally available software device instance struct to store the SPI driver state while it is in use.
struct spi_module spi_master_instance;

//A globally available peripheral slave software device instance struct.
struct spi_slave_inst slave;

static void configure_spi_master(void)
{
	struct spi_config config_spi_master;
	struct spi_slave_inst_config slave_dev_config;
	
	/* Configure and initialize software device instance of peripheral slave */
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	slave_dev_config.ss_pin = PIN_PA09;
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
}

static void init_uart0(void)
{
	struct usart_config uart_settings;
	usart_get_config_defaults(&uart_settings);
	uart_settings.mux_setting = USART_RX_1_TX_2_XCK_3;
	uart_settings.pinmux_pad0 = PINMUX_PA24C_SERCOM3_PAD2; // Tx
	uart_settings.pinmux_pad1 = PINMUX_PA23C_SERCOM3_PAD1; // Rx
	uart_settings.pinmux_pad2 = PINMUX_UNUSED;
	uart_settings.pinmux_pad3 = PINMUX_UNUSED;
	uart_settings.baudrate = 9600;
	while (usart_init(&usart_instance, SERCOM3, &uart_settings) != STATUS_OK){}
	
	stdio_serial_init(&usart_instance, SERCOM3, &uart_settings);
	usart_enable(&usart_instance);
}

struct adc_module adc_instance;

uint16_t adc_buffer[4];

uint16_t adc_avg = 0;

void adc_complete_callback(const struct adc_module *const module)
{
	//compute the average
	uint32_t avg = 0;
	for(uint8_t i=0;i<4;i++)
	{
		avg += adc_buffer[i];
	}
	
	avg >>= 2;
	
	adc_avg = avg;
	
	//do something with the average
}

void configure_adc(void)
{
	struct adc_config config_adc;
	adc_get_config_defaults(&config_adc);
	
	config_adc.gain_factor = ADC_GAIN_FACTOR_1X;
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV8;
	//Select reference
	config_adc.reference = ADC_REFCTRL_REFSEL_AREFA;
	config_adc.positive_input = ADC_POSITIVE_INPUT_PIN0;
	config_adc.resolution = ADC_RESOLUTION_16BIT;
	
	adc_init(&adc_instance, ADC, &config_adc);
	
	adc_enable(&adc_instance);
	
	adc_register_callback(&adc_instance, adc_complete_callback, ADC_CALLBACK_READ_BUFFER);
	adc_enable_callback(&adc_instance, ADC_CALLBACK_READ_BUFFER);
}

int main (void)
{
	
	system_init();
	delay_init();
	init_uart0();
	
	struct port_config pinconf;
		
	
	//led_sys
	port_get_config_defaults(&pinconf);
	pinconf.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_SYS, &pinconf);
	
	/* Configure analog pins */
	struct system_pinmux_config config;
	system_pinmux_get_config_defaults(&config);
	
	/* Analog functions are all on MUX setting B */
	config.input_pull   = SYSTEM_PINMUX_PIN_PULL_NONE;
	config.mux_position = MUX_PA03B_DAC_VREFP;
	
	system_pinmux_pin_set_config(PIN_PA03, &config);

	
	
	configure_adc();

	
	configure_spi_master();
	
	
	
	while (true) {
		/* Infinite loop */
		//if(!port_pin_get_input_level(BUTTON_0_PIN)) {
// 			spi_select_slave(&spi_master_instance, &slave, true);
// 			spi_write_buffer_wait(&spi_master_instance, buffer, BUF_LENGTH);
// 			spi_select_slave(&spi_master_instance, &slave, false);
			
			for (int i = 0; i < 8; i++)
			{
				spi_select_slave(&spi_master_instance, &slave, true);
				uint8_t buf;
				buf = 1 << i;
				spi_write_buffer_wait(&spi_master_instance, &buf, 1);
				spi_select_slave(&spi_master_instance, &slave, false);
				delay_ms(500);
			}
			
			printf("woo!!\n\r");
			adc_read_buffer_job(&adc_instance,adc_buffer,4);
			
			port_pin_set_output_level(LED_SYS, true);
			delay_ms(100);
			
			printf("Read adc value is %f\n\r", (float)adc_avg/UINT16_MAX*3.3);
			
			port_pin_set_output_level(LED_SYS, false);
			delay_ms(100);
			
		//}
	}
	
}
