/*
 * uart_service.h
 *
 * Created: 2015-10-20 15:13:33
 *  Author: petter
 */ 


#ifndef UART_SERVICE_H_
#define UART_SERVICE_H_



struct usart_module usart_instance;

//This is so basic so i wont even make a cfile
static void init_uart0(void)
{
	struct usart_config uart_settings;
	usart_get_config_defaults(&uart_settings);
	
	//Set clock to 8M
	uart_settings.generator_source = GCLK_GENERATOR_3;
	
	uart_settings.mux_setting = USART_RX_1_TX_2_XCK_3;
	uart_settings.pinmux_pad0 = PINMUX_PA24C_SERCOM3_PAD2; // Tx
	uart_settings.pinmux_pad1 = PINMUX_PA23C_SERCOM3_PAD1; // Rx
	uart_settings.pinmux_pad2 = PINMUX_UNUSED;
	uart_settings.pinmux_pad3 = PINMUX_UNUSED;
	uart_settings.baudrate = 11500;
	while (usart_init(&usart_instance, SERCOM3, &uart_settings) != STATUS_OK){}
	
	stdio_serial_init(&usart_instance, SERCOM3, &uart_settings);
	usart_enable(&usart_instance);
	
	printf("\n\rUart0 init!\n\r");
}



#endif /* UART_SERVICE_H_ */