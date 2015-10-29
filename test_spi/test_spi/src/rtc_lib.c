/*
 * rtc_lib.c
 *
 * Created: 2015-10-21 10:18:53
 *  Author: peol0071
 */ 
#include "rtc_lib.h"
#include "longboard.h"

//Private
struct rtc_module rtc_instance;

alarm_t alarm_conf[NO_RTC_ALARMS];
soft_alarm_t soft_alarm_conf[NO_SOFT_RTC_ALARMS];

uint8_t no_active_alarms;

 void rtc_lib_configure_calendar(void)
{
	/* Initialize RTC in calendar mode. */
	struct rtc_calendar_config config_rtc_calendar2;
	rtc_calendar_get_config_defaults(&config_rtc_calendar2);
	
	rtc_calendar_init(&rtc_instance, RTC, &config_rtc_calendar2);
	rtc_calendar_enable(&rtc_instance);
	
	//Set time to something more recent
	struct rtc_calendar_time time;
	rtc_calendar_get_time_defaults(&time);
	time.year = 2015;
	time.month = 10;
	time.day = 1;
	time.hour = 0;
	time.minute = 0;
	time.second = 0;
	/* Set current time. */
	rtc_calendar_set_time(&rtc_instance, &time);

}

//Update timestamp to next time
static void rtc_lib_update_alarm_time(struct rtc_calendar_time* time, alarm_t active_alarm )
{
		//Update and normalize alarm time
		time->second += active_alarm.alarm_interval_sec;
		time->second %= 60;
		time->minute += active_alarm.alarm_interval_min;
		time->minute %= 60;
		time->hour += active_alarm.alarm_interval_hour;
		time->hour %= 24;
}
static inline bool rtc_lib_compare_time(struct rtc_calendar_time* time1, struct rtc_calendar_time* time2)
{
	//Normalize am/pm
	time1->hour %= 12;
	time2->hour %= 12;	
	
	if ((time1->second == time2->second) &&
		(time1->hour == time2->hour) &&
		(time1->minute == time2->minute))
	{
		return true;
	}
	return false;
}
//handle soft alarms! This should be registered as alarm 0
//This could also be added as an invisible extension to the normal alarms
static void rtc_lib_soft_alarm_handler(void)
{
	soft_alarm_t * curr_alarm = soft_alarm_conf;
	//Get current time
	struct rtc_calendar_time now;
	rtc_calendar_get_time(&rtc_instance, &now);
	//Loop through soft alarms
	while(curr_alarm->alarm_settings.active)
	{
		if (rtc_lib_compare_time(&now, &curr_alarm->next_alarm))
		{
			//Alarm has gone off, handle!
			rtc_lib_update_alarm_time(&curr_alarm->next_alarm, curr_alarm->alarm_settings);
			curr_alarm->alarm_settings.external_callback_func();
		}
		//Move on to the next one
		curr_alarm++;
	}
	
}
//Callback function:
 static void rtc_match_callback(enum rtc_calendar_alarm alarm_number)
{
	
	/* Set new alarm in alarm_interval_sec seconds */
	static struct rtc_calendar_alarm_time alarm;
	
	//Get current time to not mess with am/pm bull
	struct rtc_calendar_time now;
	rtc_calendar_get_time(&rtc_instance, &now);
	
	alarm.time = now;
	
	//Disable updating mask
	alarm.mask = RTC_LIB_SKIP_MASK_MASK;
	
	//Update and normalize alarm time
	rtc_lib_update_alarm_time(&alarm.time, alarm_conf[alarm_number]);
	
	rtc_calendar_set_alarm(&rtc_instance, &alarm, alarm_number);
	
	//Do we have a valid callback? 
	if (*alarm_conf[alarm_number].external_callback_func)
	{
		//Call it then
		alarm_conf[alarm_number].external_callback_func();
	}
	
	//get and print time
// 	struct rtc_calendar_time test;
// 	rtc_calendar_get_time(&rtc_instance, &test);
//	printf("\talarm!\r\n\r\nklockan är %02d:%02d:%02d %04d-%02d-%02d \n\r", test.hour, test.minute, test.second, test.year, test.month, test.day);
}
//Wrappers
static void rtc_match_callback0(void)
{
	rtc_match_callback(RTC_CALENDAR_ALARM_0);
}
// static void rtc_match_callback1(void)
// {
// 	rtc_match_callback(RTC_CALENDAR_ALARM_1);
// }
// static void rtc_match_callback2(void)
// {
// 	rtc_match_callback(RTC_CALENDAR_ALARM_2);
// }
// static void rtc_match_callback3(void)
// {
// 	rtc_match_callback(RTC_CALENDAR_ALARM_3);
// }
//Set upp a callback for an alarm
 static void configure_rtc_callbacks( void(*callback_func)(void), enum rtc_calendar_alarm alarm_number)
{
	alarm_conf[alarm_number].external_callback_func = callback_func;
	
	//Just put these in a list for clean code
	void(*callback_wrappers[NO_RTC_ALARMS])(void) = {rtc_match_callback0};
	//const void(*callback_wrappers[NO_RTC_ALARMS])(void) = {rtc_match_callback0, rtc_match_callback1, rtc_match_callback2, rtc_match_callback3 };
		
	
	//And register callback
	rtc_calendar_register_callback(	&rtc_instance, callback_wrappers[alarm_number], alarm_number);
	rtc_calendar_enable_callback(&rtc_instance, alarm_number);
}

//Set RTC from arguments and do cleanup relevant to this project
//void rtc_simple_configuration(uint16_t interval, void(*callback_func)(void), enum rtc_calendar_alarm alarm_number)
void rtc_lib_set_alarm_simple(uint16_t interval, void(*callback_func)(void))
{
	//Slight hack to keep it SIMPLE
	static enum rtc_calendar_alarm alarm_number = 0;
	
	//And then set the alarm!
	rtc_lib_set_alarm(interval, callback_func, alarm_number);
	
	alarm_number++;
}
	
void rtc_lib_set_alarm(uint16_t interval, void(*callback_func)(void), enum rtc_calendar_alarm alarm_number)
{
	//Validate arguments
	if (alarm_number >= NO_RTC_ALARMS || !callback_func )
	{
		return;	//Fail
	}
	
	//Set all parts of the alarm struct. (This is to speed up the interrupt)
	alarm_conf[alarm_number].alarm_interval_hour = interval / (60 * 60);
	alarm_conf[alarm_number].alarm_interval_min = interval % (60 * 60) / 60;
	alarm_conf[alarm_number].alarm_interval_sec = interval % 60;
	
	//Set to active, for future generations
	alarm_conf[alarm_number].active = true;
	
	//Get current time to sync alarm and time
	struct rtc_calendar_time time;
	/* Get current time. */
	rtc_calendar_get_time(&rtc_instance, &time);
	
	struct rtc_calendar_alarm_time alarm;
	
	alarm.time = time;
	alarm.mask = RTC_CALENDAR_ALARM_MASK_SEC;	//Match on HH:MM:SS
	
	//Update alarm time to next time
	rtc_lib_update_alarm_time(&time, alarm_conf[alarm_number]);
	
	//Update alarm
	rtc_calendar_set_alarm(&rtc_instance, &alarm, alarm_number);		
	
	/* Configure and enable callback */
	configure_rtc_callbacks(callback_func, alarm_number);
}

//Set up handler for soft alarms
void rtc_lib_configure_soft_alarms(void)
{
	//Set up calendar and register handler for soft alarms
	rtc_lib_configure_calendar();
	//init items
	for (int i = 0; i < NO_SOFT_RTC_ALARMS;i++)
	{
		soft_alarm_conf[i].alarm_settings.active = false;
	}
	rtc_lib_set_alarm_simple(0, rtc_lib_soft_alarm_handler);
}
//Simple way to set up a soft alarm
void rtc_lib_set_soft_alarm_simple(uint16_t interval, void(*callback_func)(void))
{
	//Slight hack to keep it SIMPLE
	static uint8_t alarm_number = 0;
	
	//TODO: Write code here
	//Set all parts of the alarm struct. (This is to speed up the interrupt)
	soft_alarm_conf[alarm_number].alarm_settings.alarm_interval_hour = interval / (60 * 60);
	soft_alarm_conf[alarm_number].alarm_settings.alarm_interval_min = interval % (60 * 60) / 60;
	soft_alarm_conf[alarm_number].alarm_settings.alarm_interval_sec = interval % 60;
		
	//Set to active, for future generations
	soft_alarm_conf[alarm_number].alarm_settings.active = true;
		
	//Get current time to sync alarm and time
	struct rtc_calendar_time time;
	/* Get current time. */
	rtc_calendar_get_time(&rtc_instance, &time);
	
	//Update alarm time to next time
	rtc_lib_update_alarm_time(&time, soft_alarm_conf[alarm_number].alarm_settings);
	
	//Set alarm time to that time
	soft_alarm_conf[alarm_number].next_alarm = time;
		
	/* Configure and enable callback */
	soft_alarm_conf[alarm_number].alarm_settings.external_callback_func = callback_func;
	
	alarm_number++;
}