#include "case.h"

int16_t caseFan = defaultValue;
uint8_t caseTemp = defaultValue;
uint32_t eth_down=defaultValue; 
uint32_t eth_up=defaultValue;


void initCaseParameters()
{

}

// UPDATE /case/temp?value=XXX (10 => 10%)
int8_t setCaseTemp(String command, String &response)
{
  caseTemp = command.toInt();
  return 0;
}

// UPDATE /case/fan?value=XXX (10 => 10°)
int8_t setCaseFan(String command, String &response)
{
  caseFan = command.toInt();
  return 0;
}

// UPDATE /eth/up?value=XXX
int8_t setEthUp(String command, String &response)
{
  eth_up = command.toInt();
  return 0;
}

// UPDATE /eth/down?value=XXX
int8_t setEthDown(String command, String &response)
{
  eth_down = command.toInt();
  return 0;
}


