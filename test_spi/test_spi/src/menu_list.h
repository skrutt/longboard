/*
 * menu_list.h
 *
 * Created: 2015-10-05 14:04:35
 *  Author: jiut0001
 */ 


#ifndef MENU_LIST_H_
#define MENU_LIST_H_


struct gfx_mono_menu menu_list[] = {
// Main menu.
{"Settings", (const char*[]){"GSM", "Display", "Exit"}, 3,
(menu_link[]){GSM_MENU, DISPLAY_MENU, EXIT_MENU},
SPEED_VIEW, 0, 0},

// GSM main menu.
{"GSM", (const char*[]){"Module", "Idle mode", "Logging", "Back"}, 4,
(menu_link[]){GSM_MODULE_MENU, GSM_IDLE_MENU, GSM_LOGGING_MENU, EXIT_MENU},
MAIN_MENU, 0, 0},

// GSM power settings.
{"GSM Module",	(const char*[]){"Turn off", "Back"}, 2,
(menu_link[]){NO_EVENT, EXIT_MENU},
GSM_MENU, 0, 0},

// GSM logging frequency.
{"Logging freq.",	(const char*[]){"10 sec", "30 sec", "1 min", "5 min", "10 min", "30 min", "1 hour", "Back"}, 8,
(menu_link[]){EXIT_MENU,EXIT_MENU,EXIT_MENU,EXIT_MENU,EXIT_MENU,EXIT_MENU,EXIT_MENU,EXIT_MENU},
GSM_MENU, 0, 0},

// Idle mode menu.
{"Idle Mode",	(const char*[]){"Back"}, 1,
(menu_link[]){EXIT_MENU}, GSM_MENU, 0, 0},

// Display menu
{"Display",		(const char*[]){"Sleep mode", "Power", "Back"}, 3,
(menu_link[]){EXIT_MENU, EXIT_MENU, EXIT_MENU}, MAIN_MENU, 0, 0} //Fix sub menus
};



#endif /* MENU_LIST_H_ */