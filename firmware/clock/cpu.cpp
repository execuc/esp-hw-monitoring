#include "cpu.h"

int16_t cpuUsageGlobal = defaultValue;
int16_t cpuUsage[8] = {defaultValue, defaultValue, defaultValue,defaultValue, defaultValue, defaultValue, defaultValue, defaultValue};
int16_t cpuTempGlobal = defaultValue;
int16_t cpuTemp[8] = {defaultValue, defaultValue, defaultValue,defaultValue, defaultValue, defaultValue, defaultValue, defaultValue};
int16_t cpuFan = defaultValue;
uint32_t ramUsage = defaultValue;
uint32_t ramMax = defaultValue;
uint32_t diskUsage = defaultValue;
uint32_t diskMax = defaultValue;
int16_t otherTempGlobal = defaultValue;

int32_t ramLeft = defaultValue;
int32_t hddLeft = defaultValue;


void initCpuParameters()
{

}

static int8_t extractArray(String &command, int16_t *array)
{
    int8_t startIndex = 1, lastIndex;
    int8_t index = 0;
    bool needStop = false;
    while(index < 8)
    {
      lastIndex = command.indexOf(",", startIndex);
      if(lastIndex == -1)
      {
        lastIndex = command.length() - 1;
        needStop = true;
      }
      /*Serial.print("test");
      Serial.print(index);
      Serial.print(" ");
      Serial.println(command.substring(startIndex, lastIndex));*/
      array[index++] = command.substring(startIndex, lastIndex).toInt();
      if(needStop)
        break;
      startIndex = lastIndex +1;
    }
    return index;
}

// UPDATE /cpu/temp?value=XXX (10 => 10°)
// UPDATE /cpu/temp?value=[XXX,XXX, ...] (no space)
int8_t setCpuTemp(String command, String &response)
{
  if(command.substring(0, 1) == "[")
  {
    extractArray(command, cpuTemp);
  }
  else
  {
    cpuTempGlobal = command.toInt();
  }
  return 0;
}

// UPDATE /cpu/usage?value=XXX (10 => 10%)
// UPDATE /cpu/usage?value=[XXX,XXX, ...] (no space)
int8_t setCpuUsage(String command, String &response)
{
  if(command.substring(0, 1) == "[")
  {
    extractArray(command, cpuUsage);
  }
  else
  {
    cpuUsageGlobal = command.toInt();
  }
  return 0;
}

// UPDATE /disk/max?value=XXX (MB)
int8_t setDiskMax(String command, String &response)
{
  diskMax = command.toInt();
  return 0;
}

// UPDATE /disk/usage?value=XXX (MB)
int8_t setDiskUsage(String command, String &response)
{
  diskUsage = command.toInt();
  //hddLeft = diskMax - diskUsage;
  //TODO rename handler
  hddLeft = diskUsage;
  return 0;
}

// UPDATE /ram/max?value=XXX (MB)
int8_t setRamMax(String command, String &response)
{
  ramMax = command.toInt();
  return 0;
}

// UPDATE /ram/usage?value=XXX (MB)
int8_t setRamUsage(String command, String &response)
{
  ramUsage = command.toInt();
  //ramLeft = ramMax - ramUsage;
  //TODO rename handler
  ramLeft = ramUsage;
  return 0;
}

// UPDATE /other/temp?value=XXX
/*int8_t setOtherTemp(String command, String &response)
{
  otherTempGlobal = command.toInt();
}*/

// UPDATE /cpu/temp?value=XXX
int8_t setCpuFan(String command, String &response)
{
  cpuFan = command.toInt();
  return 0;
}

