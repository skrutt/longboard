/*
 * sim808_uart.c
 *
 * Created: 2015-10-16 09:09:14
 *  Author: jiut0001
 */ 

#include "sim808_uart.h"
#include "platform.h"

void sim808_init() {
	uint8_t success;
	gps_logging_enabled = 1;
	
	//Setup GSM key pin
	port_get_config_defaults(&pin_cfg);
	pin_cfg.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(SIM808_RESET_PIN, &pin_cfg);
	port_pin_set_output_level(SIM808_RESET_PIN, true);
	
	init_SIM808_uart();
	init_sim808_usart_callbacks();
	sim808_init_commands();
	
	system_interrupt_enable_global();
	
	usart_register_callback(&SIM808_usart, usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	usart_read_buffer_job(&SIM808_usart, (uint8_t *)incoming_byte, MAX_RX_BUFFER_LENGTH);
	
	//Check if turned off
	sim808_send_command(CMD_AT);
	if(sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL) == 0) {
		//Enable the module if turned off:
		delay_ms(400);
		port_pin_set_output_level(SIM808_RESET_PIN, false);
		delay_ms(6000);
		port_pin_set_output_level(SIM808_RESET_PIN, true);		
	}

	do {
		usart_disable_callback(&SIM808_usart, USART_CALLBACK_BUFFER_RECEIVED);
		success = 1;
		sim808_send_command(CMD_RESET);		
		delay_ms(400);
		sim808_send_command(CMD_NO_ECHO);	//Disable echo
		usart_enable_callback(&SIM808_usart, USART_CALLBACK_BUFFER_RECEIVED);
		usart_read_buffer_job(&SIM808_usart, (uint8_t *)incoming_byte, MAX_RX_BUFFER_LENGTH);
		sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
		sim808_send_command(CMD_GPS_PWR_ON);	//Enable GPS
		sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
	} while(success == 0);
	
	uint8_t connection = 1;
	do {
		sim808_init_http();
		connection = sim808_connect();	
	} while(connection == 0);


}

void sim808_reset() {
	port_pin_set_output_level(SIM808_RESET_PIN, false);
	delay_ms(3000);
	port_pin_set_output_level(SIM808_RESET_PIN, true);
	delay_ms(100);
	port_pin_set_output_level(SIM808_RESET_PIN, false);
	delay_ms(4000);
	port_pin_set_output_level(SIM808_RESET_PIN, true);
	
	sim808_send_command(CMD_GPS_PWR_ON);	//Enable GPS
	sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
	delay_ms(200);
}

void sim808_init_http() {
	volatile uint8_t result = 0;
	uint8_t fail_counter = 0;
	command cmd;
	cmd.expected_response = "OK";
	cmd.callback_enabled = 0;
	
	gprs_send_buf_init(&gprs_log_buf);
	
	do {
		result = 1;
		cmd.cmd = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"";
		sim808_send_command(cmd);
		sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
		delay_ms(400);
		
		cmd.cmd = "AT+SAPBR=3,1,\"APN\",\"online.telia.se\"";
		sim808_send_command(cmd);
		sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
		delay_ms(400);
			
		cmd.cmd = "AT+HTTPINIT";
		sim808_send_command(cmd);
		sim808_parse_response_wait(SIM808_RECEIVE_DELAY_LONG);

		cmd.cmd = "AT+HTTPPARA=\"CID\",1"; 							//Bearer profile identifier
		sim808_send_command(cmd);
		if(!sim808_parse_response_wait(SIM808_RECEIVE_DELAY_LONG)) result = 0;
	
		cmd.cmd = "AT+HTTPPARA=\"UA\",\"FONA\"";					//User agent
		sim808_send_command(cmd);
		if(!sim808_parse_response_wait(SIM808_RECEIVE_DELAY_LONG)) result = 0;
	
		cmd.cmd = "AT+HTTPPARA=\"URL\",\"http://tripcomputer.azurewebsites.net/api/datalog\"";
		sim808_send_command(cmd);
		if(!sim808_parse_response_wait(SIM808_RECEIVE_DELAY_LONG)) result = 0;
	
		cmd.cmd = "AT+HTTPPARA=\"TIMEOUT\",30";
		sim808_send_command(cmd);
		if(!sim808_parse_response_wait(SIM808_RECEIVE_DELAY_LONG)) result = 0;
		
		if(result == 0) {
			if(fail_counter >= 3) {		//Could not connect to the network.
				sim808_fail_to_connect_platform();
				fail_counter = 0;
			}		
			fail_counter++;
			sim808_reset();			
		}
	} while(result == 0);
}

uint8_t sim808_connect() {
	volatile uint8_t res = 1;
	command cmd;
	cmd.expected_response = "OK";
	cmd.callback_enabled = 0;
		
	cmd.cmd = "AT+SAPBR=0,1";									//Disconnect if connected
	sim808_send_command(cmd);
	sim808_parse_response_wait(SIM808_RECEIVE_DELAY_MEGALONG);
	delay_ms(2000);
	
	cmd.cmd = "AT+SAPBR=1,1";									//Connect to network
	sim808_send_command(cmd);
	if(!sim808_parse_response_wait(SIM808_RECEIVE_DELAY_MEGALONG)) res = 0;
	delay_ms(500);		

	return res;
}

void sim808_send_command(command cmd) {
	//uint8_t send_string_len = strlen(cmd.cmd)+2;
	//char send_string[50];
	last_command = cmd;
	printf("%s\r\n", cmd.cmd);
}

uint8_t sim808_parse_response_wait(uint16_t timeout) {
	volatile uint16_t i = 0;
	
	while(i < timeout) {
		if(SIM808_buf.available == 1) {
			return sim808_parse_response();
		}
		delay_ms(1);
		i++;
	}
	
	return 0;
	
}

uint8_t sim808_parse_response() {
	volatile uint8_t result = 0;
	volatile uint8_t cmp_cmd_len = SIM808_buf.position;
	volatile uint8_t resp_len = strlen(last_command.expected_response);
	
	volatile char originalChar = SIM808_buf.command[resp_len];
	if(cmp_cmd_len > resp_len) {
		SIM808_buf.command[resp_len] = '\0';
	}
	
	if(strcmp(SIM808_buf.command, last_command.expected_response) == 0) {
		result = 1;
	}
	
	SIM808_buf.command[resp_len] = originalChar;	//Reset to original state after comparison
	
	if(last_command.callback_enabled) {
		(*last_command.response_cb)(result, SIM808_buf.command);	
	}
	
	SIM808_buf.available = 0;
	SIM808_buf.position = 0;
	memset(SIM808_buf.command, 0, sizeof(unsigned char)*COMMAND_BUFFER_SIZE);
	
	return result;
}

void usart_read_callback(struct usart_module *const usart_module)
{
	if(incoming_byte[0] == '\n') {
		if(SIM808_buf.position > 1) {
			SIM808_buf.command[SIM808_buf.position] = '\0';
			SIM808_buf.available = 1;
		}
	}
	else if(incoming_byte[0] != '\r'){
		SIM808_buf.command[SIM808_buf.position] = incoming_byte[0];
		SIM808_buf.position++;
	}
	
	usart_read_buffer_job(&SIM808_usart, (uint8_t *)incoming_byte, MAX_RX_BUFFER_LENGTH);
}

void usart_write_callback(struct usart_module *const usart_module)
{
	//Write callback
}

void init_SIM808_uart(void) {
	struct usart_config uart_settings;
	usart_get_config_defaults(&uart_settings);
	
	//Set clock to 8M
	uart_settings.generator_source = GCLK_GENERATOR_0;
	
	uart_settings.mux_setting = USART_RX_1_TX_2_XCK_3;
	uart_settings.pinmux_pad0 = PINMUX_PA10C_SERCOM0_PAD2; // Tx
	uart_settings.pinmux_pad1 = PINMUX_PA09C_SERCOM0_PAD1; // Rx
	uart_settings.pinmux_pad2 = PINMUX_UNUSED;
	uart_settings.pinmux_pad3 = PINMUX_UNUSED;
	uart_settings.baudrate = 115200;
	while (usart_init(&SIM808_usart, SERCOM0, &uart_settings) != STATUS_OK){}
	
	stdio_serial_init(&SIM808_usart, SERCOM0, &uart_settings);
	usart_enable(&SIM808_usart);
}

void init_sim808_usart_callbacks(void)
{
	usart_register_callback(&SIM808_usart, usart_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	//usart_register_callback(&SIM808_usart, usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&SIM808_usart, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&SIM808_usart, USART_CALLBACK_BUFFER_RECEIVED);
}

void sim808_init_commands() {
	CMD_RESET.cmd = "ATZ0";
	CMD_RESET.expected_response = "OK";
	CMD_RESET.callback_enabled = 0;
	
	CMD_NO_ECHO.cmd = "ATE0";
	CMD_NO_ECHO.expected_response = "OK";
	CMD_NO_ECHO.callback_enabled = 0;
	
	CMD_GPS_PWR_ON.cmd = "AT+CGPSPWR=1";
	CMD_GPS_PWR_ON.expected_response = "OK";
	CMD_GPS_PWR_ON.callback_enabled = 0;
	
	CMD_GPS_PWR_OFF.cmd = "AT+CGPSPWR=0";
	CMD_GPS_PWR_OFF.expected_response = "OK";
	CMD_GPS_PWR_OFF.callback_enabled = 0;
	
	CMD_GET_GPS_DATA.cmd = "AT+CGPSINF=32";
	CMD_GET_GPS_DATA.callback_enabled = 1;
	CMD_GET_GPS_DATA.expected_response = "+CGPSINF";
	CMD_GET_GPS_DATA.response_cb = &SIM808_response_gps_data;
	
	CMD_GET_GPS_FIX.cmd = "AT+CGPSSTATUS?";
	CMD_GET_GPS_FIX.callback_enabled = 0;
	CMD_GET_GPS_FIX.expected_response = "Location 3D";
	
	CMD_GPRS_GET_REQ.cmd = "AT+HTTPACTION=0";
	CMD_GPRS_GET_REQ.callback_enabled = 1;
	CMD_GPRS_GET_REQ.expected_response = "OK";
	CMD_GPRS_GET_REQ.response_cb = &SIM808_response_gprs_get;
	
	CMD_GPRS_POST_REQ.cmd = "AT+HTTPACTION=1";
	CMD_GPRS_POST_REQ.callback_enabled = 0;
	CMD_GPRS_POST_REQ.expected_response = "OK";
	CMD_GPRS_POST_REQ.response_cb = &SIM808_response_gprs_post;
	
	CMD_AT.cmd = "AT";
	CMD_AT.callback_enabled = 0;
	CMD_AT.expected_response = "OK";
}