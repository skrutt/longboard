/*
 * response_actions.c
 *
 * Created: 2015-10-15 13:12:52
 *  Author: jiut0001
 */ 
#include "response_actions.h"

// Send previously added JSON POST data to web server.
void SIM808_response_gprs_send_post_request(volatile uint8_t success, volatile char *cmd) {
	if(success == 1) {
		sim808_send_command(CMD_GPRS_POST_REQ);	
		volatile uint8_t res = sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL); // Perhaps make asynchronous.
		last_command.callback_enabled = 1;
		last_command.expected_response = "+HTTPACTION";
	}
}

void SIM808_response_gprs_post(volatile uint8_t success, volatile char *cmd) {
	if(success) {
		if(strcmp(last_command.expected_response, "OK") == 0) {
			last_command.expected_response = "+HTTPACTION";
		}
		else {	 
			gprs_log_buf.ready = 1;		//The buffer is free to use again
			
			// Enable gps communication if transfer complete.
			if(gprs_log_buf.temp_tail == gprs_log_buf.head) {	
				gps_logging_enabled = 1;	
			}
			
			volatile uint8_t len = strlen(cmd);
			char *errorCodeString = cmd+15;		// Beginning of error code
			*(errorCodeString+3) = '\0';		// Close string after error code
			
			// Bättre att jämföra strängvärdet prestandamässigt?
			uint16_t errorCode = atoi(errorCodeString);	
			
			//TODO: SKA VARA 200
			if(errorCode == 400) {
				//Success
				gprs_log_buf.tail = gprs_log_buf.temp_tail;
				
			}
			else {
				//Fail
				gprs_log_buf.temp_tail = gprs_log_buf.tail;
			}
		}
	}
	else {
		// TODO: Wrong command received, handle error.
	}
}

void SIM808_response_gprs_get(volatile uint8_t success, volatile char *cmd) {
	last_command.expected_response = "+HTTPACTION";
}

//Update GPS data
// TODO: Only update if success = 1
void SIM808_response_gps_data(volatile uint8_t success, volatile char *cmd) {
	
	volatile uint8_t testVar = success;
	testVar = success;
	
	
	volatile char *comma;
	volatile char *position;
	char field[15];
	
	comma = strchr (cmd, ',');
	position = cmd;
	
	uint8_t j = 0;
	while (comma) {
		int i = 0;

		while (position < comma) {
			field[i] = *position;
			i++;
			position++;
		}
		
		// Add a NULL to the end of the string
		field[i] = '\0';
		
		
		//Bind the separated field to the gps_data struct object:
		switch(j) {
			case 1:
			gps_data.utc_time = atoi(field);
			break;
			case 2:
			gps_data.status = field[0];
			break;
			case 3:
			if(gps_data.status != 'V') {
				gps_data.lat = gps_utils_coord_to_dec(field);
			}
			else {
				gps_data.lat = 0;
			}
			break;
			case 4:
			gps_data.ns_indicator = field[0];
			break;
			case 5:
			if(gps_data.status != 'V') {
				gps_data.lng = gps_utils_coord_to_dec(field);
			}
			else {
				gps_data.lng = 0;
			}
			break;
			case 6:
			gps_data.ew_indicator = field[0];
			break;
			case 7:
			gps_data.ground_speed = atof(field)*1.852;
			break;
			case 8:
			gps_data.ground_course = atof(field);
			break;
			case 9:
			gps_data.date = atoi(field);
			break;
			case 10:

			break;
			case 11:
			gps_data.mode = field[0];
			break;
			default:
			break;
		}

		// Position is now the comma, skip it past
		position++;
		j++;
		comma = strchr (position, ',');
	}
	
	//COMMENTED ONLY FOR DEBUG
	//if(gps_data.status == 'A') {
		gps_utils_raw_data_to_send_buffer(&gprs_log_buf);
		//gps_counter++;
	//}
}
