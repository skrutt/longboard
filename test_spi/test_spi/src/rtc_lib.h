/*
 * rtc_lib.h
 *
 * Created: 2015-10-21 10:19:05
 *  Author: peol0071
 */ 
#include <asf.h>


#ifndef RTC_LIB_H_
#define RTC_LIB_H_

typedef struct
{
	//Private variable for setting interval
	uint8_t alarm_interval_sec;
	uint8_t alarm_interval_min;
	uint8_t alarm_interval_hour;
	

	//container for callback to application
	void(*external_callback_func)(void);
	
	bool active;
} alarm_t;

//Gosh darn, we only have one
#define NO_RTC_ALARMS RTC_ALARM_NUM 
//We'll have to use software alarms instead!!
#define NO_SOFT_RTC_ALARMS 4

//Use this value as the alarm mask to skip updating it, because that takes a long time!
#define RTC_LIB_SKIP_MASK_MASK RTC_MODE2_MASK_MASK

void rtc_lib_configure_calendar(void);

void rtc_lib_set_alarm_simple(uint16_t interval, void(*callback_func)(void));
void rtc_lib_set_alarm(uint16_t interval, void(*callback_func)(void), enum rtc_calendar_alarm alarm_number);

typedef struct
{
	alarm_t alarm_settings;
	struct rtc_calendar_time next_alarm;
} soft_alarm_t;


//Set up handler for soft alarms
void rtc_lib_configure_soft_alarms(void);
void rtc_lib_set_soft_alarm_simple(uint16_t interval, void(*callback_func)(void));



#endif /* RTC_LIB_H_ */