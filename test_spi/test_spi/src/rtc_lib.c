/*
 * rtc_lib.c
 *
 * Created: 2015-10-21 10:18:53
 *  Author: peol0071
 */ 
#include "rtc_lib.h"
#include "longboard.h"

//Private variable for setting interval
static uint8_t alarm_interval_sec = 1;

//container for callback to application
void(*external_callback_func)(void);

 void configure_rtc_calendar(void)
{
	/* Initialize RTC in calendar mode. */
	struct rtc_calendar_config config_rtc_calendar2;
	rtc_calendar_get_config_defaults(&config_rtc_calendar2);
	
	rtc_calendar_init(&rtc_instance, RTC, &config_rtc_calendar2);
	rtc_calendar_enable(&rtc_instance);
}
//Callback function:
 void rtc_match_callback(void)
{
	
	/* Set new alarm in alarm_interval_sec seconds */
	static struct rtc_calendar_alarm_time alarm;
	rtc_calendar_get_alarm(&rtc_instance, &alarm, RTC_CALENDAR_ALARM_0);
	//Disable updating mask
	alarm.mask = RTC_LIB_SKIP_MASK_MASK;
	alarm.time.second += alarm_interval_sec;
	alarm.time.second = alarm.time.second % 60;
	rtc_calendar_set_alarm(&rtc_instance, &alarm, RTC_CALENDAR_ALARM_0);
	
	if (*external_callback_func)
	{
		external_callback_func();
	}
	
	//get and print time
// 	struct rtc_calendar_time test;
// 	rtc_calendar_get_time(&rtc_instance, &test);
//	printf("\talarm!\r\n\r\nklockan är %02d:%02d:%02d %04d-%02d-%02d \n\r", test.hour, test.minute, test.second, test.year, test.month, test.day);
}

 void configure_rtc_callbacks( void(*callback_func)(void))
{
	external_callback_func = callback_func;
	rtc_calendar_register_callback(	&rtc_instance, rtc_match_callback, RTC_CALENDAR_CALLBACK_ALARM_0);
	rtc_calendar_enable_callback(&rtc_instance, RTC_CALENDAR_CALLBACK_ALARM_0);
}

//Set RTC from arguments and do cleanup relevant to this project
void rtc_simple_configuration(uint8_t interval, void(*callback_func)(void))
{
	//Set up RTC
	struct rtc_calendar_time time;
	rtc_calendar_get_time_defaults(&time);
	time.year = 2015;
	time.month = 10;
	time.day = 1;
	time.hour = 0;
	time.minute = 0;
	time.second = 0;
	/* Configure and enable RTC */
	configure_rtc_calendar();
	/* Configure and enable callback */
	configure_rtc_callbacks(callback_func);
	/* Set current time. */
	rtc_calendar_set_time(&rtc_instance, &time);
	struct rtc_calendar_alarm_time alarm;
	
	alarm.time = time;
	alarm.mask = RTC_CALENDAR_ALARM_MASK_SEC;	//Match only on seconds
	
	rtc_calendar_set_alarm(&rtc_instance, &alarm, RTC_CALENDAR_ALARM_0);	
}