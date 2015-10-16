/*
 * spi_sseg.c
 *
 * Created: 2015-10-16 15:41:59
 *  Author: peol0071
 */ 
#include <asf.h>


//GPIO pin to use as Slave Select.
#define SLAVE_SELECT_PIN	PIN_PA09

//A globally available software device instance struct to store the SPI driver state while it is in use.
struct spi_module spi_master_instance;

//A globally available peripheral slave software device instance struct.
struct spi_slave_inst slave;

static void spi_master_write_done(struct spi_module *const module)
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
	
	system_pinmux_pin_set_config(PIN_PA10, &config);
}

static void configure_spi_master(void)
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
