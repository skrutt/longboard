/*
 * menu_link.h
 *
 * Created: 2015-10-02 19:11:36
 *  Author: Jimmy
 */ 


#ifndef MENU_LINK_H_
#define MENU_LINK_H_

#define VIEW_MAX_INDEX 3


typedef enum {
	SPEED_VIEW,
	CADENCE_VIEW,
	INCLINATION_VIEW,
	NO_GPS_VIEW,
	MAIN_MENU,
	GSM_MENU,
	GSM_MODULE_MENU,
	GSM_LOGGING_MENU,
	GSM_IDLE_MENU,
	DISPLAY_MENU,
	NO_EVENT,
	EXIT_MENU,
} menu_link;




#endif /* MENU_LINK_H_ */