#include "gpu.h"

uint8_t gpuTempGlobal = defaultValue;
extern int16_t gpuFan = defaultValue;

void initGpuParameters()
{

}

// UPDATE /gpu/temp?value=XXX (10 => 10°)
int8_t setGpuTemp(String command, String &response)
{
    gpuTempGlobal = command.toInt();
  return 0;
}

int8_t setGpuFan(String command, String &response)
{
    gpuFan = command.toInt();
  return 0;
}

