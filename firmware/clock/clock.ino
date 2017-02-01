#include <ESP8266WiFi.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <XPT2046_Touchscreen.h>
#include "DHT.h"
#include "cpu.h"
#include "gpu.h"
#include "case.h"
#include "datetimeHandler.h"
#include "gfx_1.h"

#define CS_PIN  D0
#define TFT_DC  D1
#define TFT_CS D2
#define TFT_RESET 10
#define DHTPIN D4
#define DHTTYPE DHT22 

XPT2046_Touchscreen ts(CS_PIN);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
DHT dht(DHTPIN, DHTTYPE);


#if defined(ESP8266)
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif
#include <Wire.h>  // must be incuded here so that Arduino library object file references work

// UPDATE /rtc/time?value=15:10:02
// UPDATE /rtc/date?value=2016-10-02

// UPDATE /gpu/temp?value=XXX (10 => 10°)
// UPDATE /gpu/fan?value=XXX 

// UPDATE /cpu/temp?value=XXX (10 => 10°)
// UPDATE /cpu/fan?value=XXX
// UPDATE /cpu/temp?value=[XXX,XXX, ...] (no space)
// UPDATE /cpu/usage?value=XXX (10 => 10%)
// UPDATE /cpu/usage?value=[XXX,XXX, ...] (no space)

// UPDATE /disk/max?value=XXX (MB)
// UPDATE /disk/usage?value=XXX (MB)

// UPDATE /ram/max?value=XXX (MB)
// UPDATE /ram/usage?value=XXX (MB)

// UPDATE /case/temp?value=XXX
// UPDATE /case/fan?value=XXX

// UPDATE /eth/up?value=XXX
// UPDATE /eth/down?value=XXX

static uint32_t tempTime  = 0; 
static uint32_t rtcTime  = 0;
static uint32_t displayTime = 0;
static String serialIntput, response;

void setup() 
{
  WiFi.disconnect(); 
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  delay(1);

  initHardware();
  tft.begin();
  tft.setRotation(3);
  ts.begin();
  initCpuParameters();
  dht.begin();
  gfx1_layout();
  Serial.write("init finished");
}

void endLcdCommand()
{
  Serial1.write(0xFF);Serial1.write(0xFF);Serial1.write(0xFF);
}

bool readSerial()
{  
  bool newMessage = false;
  while (Serial.available()) {
    char c = Serial.read();  //gets one byte from serial buffer
    if(c == '\r')
      continue;
    if(c != '\n')
      serialIntput += c; //makes the String readString
    else
      newMessage = true;
  }
  return newMessage;
}


// UPDATE
int8_t update(String command, String &response)
{
  int8_t status = -1;
  //Serial.println(command);
  String subCommand11 = command.substring(0, 11);

  if(subCommand11 == "/cpu/temp?v")
  {
    status = setCpuTemp(command.substring(16), response);
  }
  else if(subCommand11 == "/cpu/usage?")
  {
    status = setCpuUsage(command.substring(17), response);
  }
  else if(subCommand11 == "/cpu/fan?va")
  {
    status = setCpuFan(command.substring(15), response);
  }
  else if(subCommand11 == "/gpu/fan?va")
  {
    status = setGpuFan(command.substring(15), response);
  }
  else if(subCommand11 == "/gpu/temp?v")
  {
    status = setGpuTemp(command.substring(16), response);
  }
  else if(subCommand11 == "/ram/usage?")
  {
    status = setRamUsage(command.substring(17), response);
  }
  else if(subCommand11 == "/disk/usage")
  {
    status = setDiskUsage(command.substring(18), response);
  }
  else if(subCommand11 == "/disk/max?v")
  {
    status = setDiskMax(command.substring(16), response);
  }
  else if(subCommand11 == "/ram/max?va")
  {
    status = setRamMax(command.substring(15), response);
  }
  else if(subCommand11 == "/case/temp?")
  {
    status = setCaseTemp(command.substring(17), response);
  }
  else if(subCommand11 == "/eth/down?v")
  {
    status = setEthDown(command.substring(16), response);
  }
  else if(subCommand11 == "/eth/up?val")
  {
    status = setEthUp(command.substring(14), response);
  }
  else if(subCommand11 == "/case/fan?v")
  {
    status = setCaseFan(command.substring(16), response);
  }
  else if(subCommand11 == "/disk/usage")
  {
    status = setCpuUsage(command.substring(18), response);
  }
  else if(subCommand11 == "/rtc/date?v")
  {
    status = setDate(command.substring(16), response);
  }
  else if(subCommand11 == "/rtc/time?v")
  {
    status = setTime(command.substring(16), response);
  }
  else
  {
    status = -1;
    response = "\"msg\":\"Unknown command\"";
  }
  return status;
}


bool handleMessage(String &command, String &response)
{
  int8_t status = -1;
  response = "";
  //Serial.println(command);
  if(command.substring(0, 6) == "UPDATE")
  {
    status = update(command.substring(7), response);
    gfx1_refresh = true;
    gfx1_udpate();
  }
  /*else if(command.substring(0, 3) == "GET")
  {
    status = getDateTime(command.substring(4), response);
  }*/
  else
  {
    status = -1;
    response = "\"msg\":\"Unknown command\"";
  }

  if(status == 0)
  {
    response = "OK";
  }
  
  return (status == 0);
}


boolean wastouched = true;

void loop() 
{
  boolean istouched = ts.touched();
  if (istouched) {
    wastouched = true;
  }
  else
    wastouched = false;
  
  uint32_t currentTime = millis();
  if ( currentTime > tempTime ) {
      tempTime = currentTime + 10000;
      if(!isnan(dht.readTemperature()))
      {
        localHum = dht.readHumidity();
        localTemp = dht.readTemperature() - 4;
      }
      else
      {
        localHum = 255;
        localTemp = 255;
      }
      gfx1_udpate();
      return;
  }

  if ( currentTime > displayTime ) {
    displayTime = currentTime + 100000;
  }
  
  if(readSerial())
  {
    handleMessage(serialIntput, response);
    Serial.println(response);
    serialIntput = "";
    //gfx1_udpate();
  }
}


void initHardware()
{
  Serial.begin(115200);
  delay(1000);
  pinMode(TFT_RESET, OUTPUT);
  digitalWrite(TFT_RESET, LOW);
  delay(1000);
  digitalWrite(TFT_RESET, HIGH);
  delay(1000);
}

