#ifndef __CASE_H__
#define __CASE_H__

#include<Arduino.h>

#define defaultValue 0

extern void initCaseParameters();
extern int8_t setCaseTemp(String command, String &response);
extern int8_t setCaseFan(String command, String &response);
extern int8_t setEthDown(String command, String &response);
extern int8_t setEthUp(String command, String &response);

extern int16_t caseFan;
extern uint8_t caseTemp;
extern uint32_t eth_down, eth_up;

#endif // __CASE_H__
