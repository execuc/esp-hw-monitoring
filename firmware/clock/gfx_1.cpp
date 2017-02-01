#include "gfx_1.h"
#include "datetimeHandler.h"
#include "cpu.h"
#include "gpu.h"
#include "case.h"
#include "Adafruit_GFX.h"

bool gfx1_refresh = true;
static uint8_t s_time_tab[6] = {'0', '0', ':', '0', '0', 0};
static uint8_t s_localHum = 0, s_localTemp = 0, s_cpuTemp = 255, s_gpu_temp = 255, s_case_temp=255;
static int16_t s_cpu_fan = 255, s_cpu_usage = 255, s_case_fan = 255, s_gpu_fan = 255;
static uint32_t s_ram_left = 255, s_hdd_left = 255;
static uint32_t s_eth_down = 255, s_eth_up = 255;

// 5px size
static void gfx_update_toolbar();
static void gfx_update_cpu();
static void gfx_update_gpu();
static void gfx_update_mem();
static void gfx_update_case();
static void gfx_update_eth();

static void writeArray(const char *buf)
{
  uint8_t c= 0;
  while(buf[c] != 0)
    tft.write(buf[c++]);
}


void gfx1_udpate()
{
  if(!gfx1_refresh)
    return ;

  gfx1_refresh = false;
  gfx_update_toolbar();
  gfx_update_cpu();
  gfx_update_gpu();
  gfx_update_mem();
  gfx_update_case();
  gfx_update_eth();
}

#define LINE_Y_REL 29
#define TEXT_Y_REL 9
#define FIRST_TEXT_Y_REL 40
#define TEXT_ESPACE_Y 35
#define TEXT_ESPACE_SHORT_Y 25

char buf[20];

void gfx1_layout()
{
  uint8_t c= 0;
  
  tft.fillScreen(ILI9341_BLACK);
  tft.drawLine(10, 18, 310, 18, ILI9341_YELLOW);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); 
  tft.setCursor(35, 0);
  tft.setTextSize(1);
  tft.write('o');
  tft.setTextSize(2);
  tft.setCursor(35+8, 2);
  tft.write('C');
  //Humidity
  tft.setCursor(300, 2);
  tft.write('%');

  // CPU rect
  tft.setTextSize(2);
  tft.drawRoundRect(5, 25, 150, 115, 10, ILI9341_RED);
  tft.drawLine(5, 25+LINE_Y_REL, 154, 25+LINE_Y_REL, ILI9341_RED);
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK); 
  tft.setCursor(60, 25+TEXT_Y_REL);
  sprintf(buf, "CPU");
  writeArray(buf);
  tft.setCursor(15, 25+FIRST_TEXT_Y_REL);
  sprintf(buf, "Usage:");
  writeArray(buf);
  tft.setCursor(15, 25+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y);
  sprintf(buf, "Temp:");
  writeArray(buf);
  tft.setCursor(15, 25+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y *2);
  sprintf(buf, "Fan:");
  writeArray(buf);

  // GPU
  tft.setTextSize(2);
  tft.drawRoundRect(5, 145, 150, 90, 10, ILI9341_ORANGE);
  tft.drawLine(5, 145+LINE_Y_REL, 154, 145+LINE_Y_REL, ILI9341_ORANGE);
  tft.setTextColor(ILI9341_ORANGE, ILI9341_BLACK);
  tft.setCursor(60, 145+TEXT_Y_REL);
  sprintf(buf, "GPU");
  writeArray(buf);
  tft.setCursor(15, 145+FIRST_TEXT_Y_REL);
  sprintf(buf, "Temp:");
  writeArray(buf);
  tft.setCursor(15, 145+FIRST_TEXT_Y_REL + + TEXT_ESPACE_SHORT_Y);
  sprintf(buf, "Fan:");
  writeArray(buf);
  

  // memory
  tft.drawRoundRect(165, 25, 150, 88, 10, ILI9341_GREEN);
  tft.drawLine(165, 25+LINE_Y_REL, 314, 25+LINE_Y_REL, ILI9341_GREEN);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK); 
  tft.setCursor(205, 25+TEXT_Y_REL);
  sprintf(buf, "MEMORY");
  writeArray(buf);
  tft.setCursor(175, 25+FIRST_TEXT_Y_REL);
  sprintf(buf, "Ram:");
  writeArray(buf);
  tft.setCursor(175, 25+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y);
  sprintf(buf, "Hdd:");
  writeArray(buf);

  // case
  tft.drawRoundRect(165, 118, 150, 85, 10, ILI9341_CYAN);
  tft.drawLine(165, 118+LINE_Y_REL, 314, 118+LINE_Y_REL, ILI9341_CYAN);
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK); 
  tft.setCursor(220, 116+TEXT_Y_REL);
  sprintf(buf, "CASE");
  writeArray(buf);
  tft.setCursor(175, 116+FIRST_TEXT_Y_REL);
  sprintf(buf, "Fan:");
  writeArray(buf);
  tft.setCursor(175, 116+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y);
  sprintf(buf, "Temp:");
  writeArray(buf); 

  // eth
  tft.drawRoundRect(165, 208, 150, 27, 10, ILI9341_MAGENTA);
  tft.fillTriangle(225, 122+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y*2 + 3, 
                 235, 122+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y*2 + 3, 
                 230, 122+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y*2 + 3 + 14, ILI9341_MAGENTA);
  tft.fillTriangle(298, 122+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y*2 + 3 + 14, 
                 308, 122+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y*2 + 3 + 14, 
                 303, 122+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y*2 + 3, ILI9341_MAGENTA);
  //gfx1_udpate();
}

void get_formated_space(int32_t valueMb)
{
  int8_t unity = 0;
  if(valueMb > 9999)
  {
    valueMb = valueMb / 1000;
    unity = 1; // GB
  }

  if(valueMb > 9999)
  {
    valueMb = valueMb / 1000;
    unity = 2; // TB
  }

  if(unity == 0)
    sprintf(buf, "%4d MB", valueMb);
  else if(unity == 1)
    sprintf(buf, "%4d GB", valueMb);
  else
    sprintf(buf, "%4d TB", valueMb);
}

static void gfx_update_mem()
{
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK); 
  if(s_ram_left != ramLeft)
  {
    tft.setCursor(175 + 50, 25+FIRST_TEXT_Y_REL);
    get_formated_space(ramLeft);
    writeArray(buf);
    s_ram_left = ramLeft;
  }

  if(s_hdd_left != hddLeft)
  {
    tft.setCursor(175 + 50, 25+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y);
    get_formated_space(hddLeft);
    writeArray(buf);
    s_hdd_left = hddLeft;
  }
}

static void gfx_update_eth()
{
  tft.setTextColor(ILI9341_MAGENTA, ILI9341_BLACK); 
  if(s_eth_down != eth_down)
  {
    s_eth_down = eth_down;
    if(s_eth_down > 9999)
      s_eth_down = 9999;
    tft.setCursor(175, 122+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y*2 + 3);
    //sprintf(buf, "%4dD", s_eth_down);
    sprintf(buf, "%4d", s_eth_down);
    writeArray(buf);
  }

  if(s_eth_up != eth_up)
  {
    s_eth_up = eth_up;
    if(s_eth_up > 9999)
      s_eth_up = 9999;
    tft.setCursor(175 + 75, 122+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y*2 + 3);
    //sprintf(buf, "%4dU", s_eth_up);
    sprintf(buf, "%4d", s_eth_up);
    writeArray(buf);
  } 
}

static void gfx_update_case()
{
  tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK); 
  if(s_case_fan != caseFan)
  {
    tft.setCursor(175 + 64, 116+FIRST_TEXT_Y_REL);
    sprintf(buf, "%4d R", caseFan);
    writeArray(buf);
    s_case_fan = caseFan;
  }
  if(s_case_temp != caseTemp)
  {
    tft.setCursor(175 + 85, 116+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y);
    sprintf(buf, "%2d", caseTemp);
    writeArray(buf);
    tft.setCursor(175 + 115, 116+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y);
    tft.setTextSize(1);
    tft.write('o');
    tft.setTextSize(2);
    tft.setCursor(175 + 115 +8, 116+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y);
    tft.write('C');
    s_case_temp = caseTemp;
  }
}


//s_cpu_fan = 0, s_cpu_usage = 0, s_case_fan = 0;
static void gfx_update_cpu()
{
  tft.setTextColor(ILI9341_RED, ILI9341_BLACK); 
  if(cpuUsageGlobal != s_cpu_usage)
  {
    tft.setCursor(15+75, 25+FIRST_TEXT_Y_REL);
    sprintf(buf, "%3d %%", cpuUsageGlobal);
    writeArray(buf);
    s_cpu_usage = cpuUsageGlobal;
  }
  if(s_cpuTemp != cpuTempGlobal)
  {
    tft.setCursor(15 + 85, 25+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y);
    sprintf(buf, "%2d", cpuTempGlobal);
    writeArray(buf);
    tft.setCursor(15 + 115, 25+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y - 2);
    tft.setTextSize(1);
    tft.write('o');
    tft.setTextSize(2);
    tft.setCursor(15 + 115 +8, 25+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y);
    tft.write('C');
    s_cpuTemp = cpuTempGlobal;
  }
  if(s_cpu_fan != cpuFan)
  {
    tft.setCursor(15 + 60, 25+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y *2);
    sprintf(buf, "%4d R", cpuFan);
    writeArray(buf);
    s_cpu_fan = cpuFan;
  }
}

static void gfx_update_gpu()
{
  tft.setTextColor(ILI9341_ORANGE, ILI9341_BLACK); 
  if(s_gpu_temp != gpuTempGlobal)
  {
    tft.setCursor(15 + 85, 145+FIRST_TEXT_Y_REL);
    sprintf(buf, "%2d", gpuTempGlobal);
    writeArray(buf);
    tft.setCursor(15 + 115, 145+FIRST_TEXT_Y_REL - 2);
    tft.setTextSize(1);
    tft.write('o');
    tft.setTextSize(2);
    tft.setCursor(15 + 115 +8, 145+FIRST_TEXT_Y_REL);
    tft.write('C');
    s_gpu_temp  = gpuTempGlobal;
  }

  if(s_gpu_fan != gpuFan)
  {
    tft.setCursor(15 + 85, 145+FIRST_TEXT_Y_REL + TEXT_ESPACE_SHORT_Y);
    sprintf(buf, "%2d %%", gpuFan);
    writeArray(buf);
    s_gpu_fan = gpuFan;
  }
}

static void gfx_update_toolbar()
{
  uint8_t c= 0;
  char buf[5];
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK); 
 //time
 if(s_time_tab[4] != time_tab[4] || s_time_tab[3] != time_tab[3] 
  || s_time_tab[1] != time_tab[1] || s_time_tab[0] != time_tab[0])
 {
  tft.setTextSize(2);
  tft.setCursor(135, 2);
  
  while(c < 5) 
  {
    uint8_t car = time_tab[c];
    s_time_tab[c] = car;
    tft.write(car);
    c++; 
  }
 }

  //temp
  if(s_localTemp != localTemp)
  {
    tft.setCursor(10, 2);
    if(localTemp != 255)
      sprintf(buf, "%2d", localTemp);
    else
      sprintf(buf, "--");
    writeArray(buf);
    s_localTemp = localTemp;
  }
  
  // humidity
  if(s_localHum != localHum)
  {
    tft.setCursor(275, 2);
    if(localHum != 255)
      sprintf(buf, "%2d", localHum);
    else
      sprintf(buf, "--");
    writeArray(buf);
    s_localHum = localHum;
  }
}
