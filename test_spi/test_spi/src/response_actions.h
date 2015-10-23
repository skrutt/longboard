/*
 * response_actions.h
 *
 * Created: 2015-10-15 13:13:05
 *  Author: jiut0001
 */ 


#ifndef RESPONSE_ACTIONS_H_
#define RESPONSE_ACTIONS_H_

#include <asf.h>
#include "globals.h"
#include "gps_utils.h"
#include "sim808_uart.h"

void SIM808_handle_data_transfer(void);

void SIM808_response_gprs_send_post_request(volatile uint8_t, volatile char*);
void SIM808_response_gprs_post(volatile uint8_t, volatile char*);
void SIM808_response_gprs_get(volatile uint8_t, volatile char*);
void SIM808_response_gps_data(volatile uint8_t, volatile char*);


#endif /* RESPONSE_ACTIONS_H_ */