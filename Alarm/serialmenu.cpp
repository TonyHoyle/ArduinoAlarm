#include "Arduino.h"
#include <Ethernet.h>
#include <Time.h>
#include "sensors.h"
#include "serialmenu.h"

SerialMenuClass SerialMenu;

void SerialMenuClass::begin()
{
  _alarm = false;
  _strobe = false;
  
  showMenu();
}

void SerialMenuClass::maintain()
{
  if(Serial.available())
  {
    switch(Serial.read())
    {
      case '\xd': showMenu(); break;
      case '1': toggleStrobe(); break;
      case '2': toggleAlarm(); break;
    }
  }
}

void SerialMenuClass::showMenu()
{
  Serial.print(F("\033c"));
  Serial.println(F("Alarm Menu"));
  Serial.println(F("----------"));
  Serial.print(F("IP: "));
  Serial.println(Ethernet.localIP());
  Serial.print(F("Date: "));
  Serial.println(getTime());
  Serial.print(F("Memory: "));
  Serial.println(freeRam());
  Serial.print(F("Sensors: "));
  Serial.print(Sensors.sensor()|512, BIN);
  Serial.print(F(" "));
  Serial.println(Sensors.tamper()|512, BIN);
  Serial.println(F(""));
  Serial.println(_strobe?F("1: Disable strobe"):F("1: Enable strobe"));
  Serial.println(_alarm?F("2: Disable alarm"):F("2: Enable alarm"));
  Serial.println(F(""));
  Serial.println(F("Select: "));
}

int SerialMenuClass::freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

const char *SerialMenuClass::getTime()
{
  static char s_time[64];
  static char s_day[4];

  strncpy(s_day, dayShortStr(weekday()), sizeof(s_day));
  snprintf(s_time, sizeof(s_time), "%s %02d %s %04d %2d:%02d %s", s_day, day(), monthShortStr(month()), year(), hourFormat12(), minute(), isAM() ? "AM" : "PM");
  return s_time;
}

void SerialMenuClass::toggleStrobe()
{
  _strobe = !_strobe;
  Sensors.strobe(_strobe);
}

void SerialMenuClass::toggleAlarm()
{
  _alarm = !_alarm;
  Sensors.bell(_alarm);
}

