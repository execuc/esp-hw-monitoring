#include "datetimeHandler.h"

uint8_t localTemp = 255;
uint8_t localHum = 255;
uint8_t hour = 0;
uint8_t minute = 0;
uint8_t time_tab[6] = {'-', '-', ':', '-', '-', 0};

// UPDATE /rtc/date?value=2016-10-02
int8_t setDate(String date, String &response)
{  
  int8_t status = -1;
  //Serial.println(date);
  if(date.length() == 10)
  {
    /*uint16_t year = date.substring(0,4).toInt();
    uint8_t month = date.substring(5,7).toInt();
    uint8_t day = date.substring(8,10).toInt();
    // Add values protections
    RtcDateTime now = Rtc.GetDateTime();
    RtcDateTime newDateTime(year, month, day, now.Hour(), now.Minute(), now.Second());
    Rtc.SetDateTime(newDateTime);*/
    status = 0;
  }
  
  if(status == -1)
    response = "{\"msg\":\"Bad format YYYYMMDD\"}";
  return status;
}

// UPDATE /rtc/time?value=15:10:02
int8_t setTime(String time, String &response)
{
  int8_t status = -1;
  if(time.length() == 8)
  {
    hour = time.substring(0,2).toInt();
    //Serial.println(time.substring(0,5));
    time.substring(0,5).getBytes(time_tab, 5+1);
    minute = time.substring(3,5).toInt();
    /*uint8_t hour = time.substring(0,2).toInt();
    uint8_t minute = time.substring(3,5).toInt();
    uint8_t second = time.substring(6,8).toInt();
    // Add values protections
    RtcDateTime now = Rtc.GetDateTime();
    RtcDateTime newDateTime(now.Year(), now.Month(), now.Day(), hour, minute, second);
    Rtc.SetDateTime(newDateTime);*/
    status = 0;
  }
  
  if(status == -1)
    response = "\"msg\":\"Bad format YYYYMMDD\"";
  return status;
}

// GET /rtc/time
// GET /rtc/date
// GET /temp
/*int8_t getDateTime(String command, String &response)
{
  int8_t status = -1;
  Serial.println(command);
  if(command.substring(0, 9) == "/rtc/date")
  {
    RtcDateTime now = Rtc.GetDateTime();
    response = String("\"msg\":\"");
    response += String(now.Year()) + String("-") + String(now.Month()) + String("-") + String(now.Day());
    response += String("\"");
    status = 0;
  }
  else if(command.substring(0, 9) == "/rtc/time")
  {
    RtcDateTime now = Rtc.GetDateTime();
    response = String("\"msg\":\"");
    response += String(now.Hour()) + String(":") + String(now.Minute()) + String(":") + String(now.Second());
    response += String("\"");
    status = 0;
  }
  else if(command.substring(0, 5) == "/temp")
  {
    
    response = String("\"msg\":\"") + String(localTemp) + String("\"");
    status = 0;
  }
  else
  {
    status = -1;
    response = "{\"msg\":\"Unknown command\"}";
  }
  return status;
}*/

