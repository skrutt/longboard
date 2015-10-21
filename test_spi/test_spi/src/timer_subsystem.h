/*
 * timer_subsystem.h
 *
 * Created: 2015-10-19 14:43:06
 *  Author: peol0071
 */ 


#ifndef TIMER_SUBSYSTEM_H_
#define TIMER_SUBSYSTEM_H_

//Module to config timer
struct tc_module display_timer_instance;
struct tc_module logger_timer_instance;


void configure_tc(void);
void configure_tc_callbacks(tc_callback_t callback_func);




#endif /* TIMER_SUBSYSTEM_H_ */