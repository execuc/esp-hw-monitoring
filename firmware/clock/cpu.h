#ifndef __CPU_H__
#define __CPU_H__

#include<Arduino.h>

#define defaultValue 0

extern void initCpuParameters();
extern int8_t setCpuTemp(String command, String &response);
extern int8_t setCpuUsage(String command, String &response);
extern int8_t setDiskMax(String command, String &response);
extern int8_t setDiskUsage(String command, String &response);
extern int8_t setRamMax(String command, String &response);
extern int8_t setRamUsage(String command, String &response);
//extern int8_t setOtherTemp(String command, String &response);
extern int8_t setCpuFan(String command, String &response);

extern int16_t cpuUsageGlobal;
extern int16_t cpuUsage[8];
extern int16_t cpuTempGlobal;
extern int16_t cpuTemp[8];
//extern int16_t otherTempGlobal;
extern int16_t cpuFan;
extern uint32_t ramUsage;
extern uint32_t ramMax;
extern uint32_t diskUsage;
extern uint32_t diskMax;

extern int32_t ramLeft;
extern int32_t hddLeft;

#endif // __CPU_H__
