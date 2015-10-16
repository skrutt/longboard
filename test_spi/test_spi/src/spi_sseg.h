/*
 * spi_sseg.h
 *
 * Created: 2015-10-16 15:42:34
 *  Author: peol0071
 */ 


#ifndef SPI_SSEG_H_
#define SPI_SSEG_H_


//GPIO pin to use as Slave Select.
#define SLAVE_SELECT_PIN	PIN_PA09


//A globally available software device instance struct to store the SPI driver state while it is in use.
struct spi_module spi_master_instance;

//A globally available peripheral slave software device instance struct.
struct spi_slave_inst slave;

static void spi_master_write_done(struct spi_module *const module);
static void configure_spi_master(void);


#endif /* SPI_SSEG_H_ */