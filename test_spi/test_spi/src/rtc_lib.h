/*
 * rtc_lib.h
 *
 * Created: 2015-10-21 10:19:05
 *  Author: peol0071
 */ 
#include <asf.h>


#ifndef RTC_LIB_H_
#define RTC_LIB_H_

struct rtc_module rtc_instance;

//Use this value as the alarm mask to skip updating it, because that takes a long time!
#define RTC_LIB_SKIP_MASK_MASK RTC_MODE2_MASK_MASK

 void configure_rtc_calendar(void);

 void configure_rtc_callbacks(void(*callback_func)(void));

//Callback function:
 void rtc_match_callback(void);

void rtc_simple_configuration(uint8_t interval, void(*callback_func)(void));


#endif /* RTC_LIB_H_ */