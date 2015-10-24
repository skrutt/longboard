/*
 * gprs_transfer_packages.c
 *
 * Created: 2015-10-19 14:51:24
 *  Author: jiut0001
 */ 
#include "gprs_transfer_packages.h"

// Called in main loop to handle data transfer.
void SIM808_handle_data_transfer() {
	
	// Continue sending remaining packages if any.
	if(!gps_logging_enabled && gprs_log_buf.ready) {
		//gprs_send_data_log();	//COMMENTED AWAY only for DEBUG
	}
	
	// TODO: Should be taken care of by an alarm:
	if(gps_counter >= 4) {
		gps_counter = 0;
		//gprs_send_data_log();
	}
}

// Add json variable to string.
void json_add_variable(char *target, uint16_t *target_pos, const char *variable, char *value, uint8_t first) {
	
	if(first != 1) {
		sprintf((target + (*target_pos)++), ",");
	}
	
	sprintf((target + (*target_pos)), "\"%s\":", variable);
	*target_pos += 3 + strlen(variable);
	
	if(value != '\0') {
		sprintf((target + (*target_pos)), "%s", value);
		*target_pos += strlen(value);
	}

}

static inline void json_begin_array(char *target, uint16_t *target_pos, uint8_t first) {
	if(!first) sprintf((target + (*target_pos)++), ",");
	sprintf((target + (*target_pos)++), "[");
}
static inline void json_close_array(char *target, uint16_t *target_pos) {
	sprintf((target + (*target_pos)++), "]");
}
static inline void json_begin_object(char *target, uint16_t *target_pos, uint8_t first) {
	if(!first) sprintf((target + (*target_pos)++), ",");
	sprintf((target + (*target_pos)++), "{");
}
static inline void json_close_object(char *target, uint16_t *target_pos) {
	sprintf((target + (*target_pos)++), "}");
}

void gprs_send_data_log() {
	
	gps_logging_enabled = 0;	//Disable gps request commands, enabled again in post request callback.
	gprs_log_buf.ready = 0;
	
	char send_string[HTTP_PACKAGE_STRING_LENGTH];
	uint16_t pos = 0;
	log_entry entry;
	char tempVar[15];
	uint8_t i = 0;

/*
	sprintf(tempVar, "\"%s\"", gprs_log_buf.data.date_time);
	json_add_variable(&send_string, &pos, "DateTime", tempVar, 1);
	*/

	memset(send_string, '\0', HTTP_PACKAGE_STRING_LENGTH);	//Clear buffer
		
	json_begin_object(send_string, &pos, !i);
	
	sprintf(tempVar, "%d", gprs_log_buf.data.device);
	json_add_variable(send_string, &pos, "Device", tempVar, 1);
	
	json_add_variable(send_string, &pos, "Entries", '\0', 0);
	
	json_begin_array(send_string, &pos, 1);
	
	while(i < HTTP_PACKAGE_MAX_LOG_ENTRIES && gprs_log_buf.head != gprs_log_buf.temp_tail) {
		
		entry = gprs_buf_temp_pull(&gprs_log_buf);
		
		json_begin_object(send_string, &pos, !i);
		
		sprintf(tempVar, "%d", entry.time);
		json_add_variable(send_string, &pos, "t", tempVar, 1);
		
		sprintf(tempVar, "%.5f", entry.lat);
		json_add_variable(send_string, &pos, "la", tempVar, 0);
		
		sprintf(tempVar, "%.5f", entry.lng);
		json_add_variable(send_string, &pos, "ln", tempVar, 0);
		
		sprintf(tempVar, "%.1f", entry.speed);
		json_add_variable(send_string, &pos, "s", tempVar, 0);
		
		sprintf(tempVar, "%d", entry.cadence);
		json_add_variable(send_string, &pos, "c", tempVar, 0);
		
		sprintf(tempVar, "%d", entry.inclination);
		json_add_variable(send_string, &pos, "i", tempVar, 0);
		
		sprintf(tempVar, "%.1f", entry.g_force);
		json_add_variable(send_string, &pos, "g", tempVar, 0);
		
		sprintf(tempVar, "%d", entry.upload_interval);
		json_add_variable(send_string, &pos, "f", tempVar, 0);
		
		json_close_object(send_string, &pos);
		
		i++;
	}
	
	json_close_array(send_string, &pos);
	json_close_object(send_string, &pos);
		
	i = 0;
	
	// ONLY FOR DEBUG:
	char *completeString;
	volatile uint16_t len;
	completeString = &send_string;
	len = strlen(completeString);
	
	// TODO: Send post request to server:		
	command cmd;
	char cmd_name[25];
		
	sprintf(cmd_name, "AT+HTTPDATA=%d,30000", strlen(send_string));
	cmd.cmd = cmd_name;
	cmd.expected_response = "DOWNLOAD";
	cmd.callback_enabled = 0;
		
	sim808_send_command(cmd);
	if(sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL)) {
		printf(send_string);
		last_command.expected_response = "OK";
		last_command.callback_enabled = 1;
		last_command.response_cb = SIM808_response_gprs_send_post_request;
		//sim808_parse_response_wait(SIM808_RECEIVE_DELAY_NORMAL);
	}
}

void gprs_send_buf_init(gprs_send_buffer *buf) {
	buf->len = 299;
	buf->temp_tail = 0;
	buf->tail = 0;
	buf->head = 0;
	buf->ready = 1;
}

void gprs_buf_push(log_entry entry, gprs_send_buffer *buf) {
	buf->data.entries[buf->head] = entry;
	buf->head++;
	if(buf->head == buf->len) buf->head = 0;
}

log_entry gprs_buf_temp_pull(gprs_send_buffer *buf) {
	log_entry entry2 = buf->data.entries[buf->temp_tail];
	buf->temp_tail++;
	if(buf->temp_tail == buf->len) buf->temp_tail = 0;
	
	return entry2;
}

log_entry gprs_buf_pull(gprs_send_buffer *buf) {
	log_entry entry2 = buf->data.entries[buf->tail];
	buf->tail++;
	if(buf->tail == buf->len) buf->tail = 0;
	
	return entry2;
}
