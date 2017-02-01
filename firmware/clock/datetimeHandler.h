#ifndef __DATETIMEHANDLER_H__
#define __DATETIMEHANDLER_H__

#include<Arduino.h>

extern uint8_t localTemp;
extern uint8_t localHum;
extern uint8_t hour;
extern uint8_t minute;
extern uint8_t time_tab[6];
extern int8_t setDate(String date, String &response);
extern int8_t setTime(String time, String &response);
//extern int8_t getDateTime(String command, String &response);

#endif // __DATETIMEHANDLER_H__
