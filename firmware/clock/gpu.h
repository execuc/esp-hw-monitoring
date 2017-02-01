#ifndef __GPU_H__
#define __GPU_H__

#include<Arduino.h>

#define defaultValue 0

extern uint8_t gpuTempGlobal;
extern int16_t gpuFan;

extern void initGpuParameters();
extern int8_t setGpuTemp(String command, String &response);
extern int8_t setGpuFan(String command, String &response);


#endif // __GPU_H__
