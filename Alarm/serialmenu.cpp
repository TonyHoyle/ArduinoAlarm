#include "Arduino.h"
#include <Ethernet.h>
#include <Time.h>
#include "sensors.h"
#include "menu.h"
#include "bell.h"
#include "serialmenu.h"

SerialMenuClass SerialMenu;

void SerialMenuClass::begin()
{
  showMenu();
}

void SerialMenuClass::maintain()
{
  if(Serial.available())
  {
    String line = Serial.readStringUntil('\n');
    switch(line[0])
    {
      case '\xd': showMenu(); break;
      case '1': toggleStrobe(); break;
      case '2': toggleAlarm(); break;
      case '3': toggleTestMode(); break;
      case '4': toggleArmed(); break;
      case 'p': setPin(line.c_str()+1); break;
    }
    // Sink any remaining characters just in case
    while(Serial.available()) 
      Serial.read();
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
  Serial.print(F("Pin: "));
  Serial.write(Menu.getPin(), Menu.getPinLength());
  Serial.println(F(""));
  Serial.println(F(""));
  Serial.println(Bell.strobe()?F("1: Disable strobe"):F("1: Enable strobe"));
  Serial.println(Bell.bell()?F("2: Disable alarm"):F("2: Enable alarm"));
  Serial.println(Bell.testMode()?F("3: Disable test mode"):F("3: Enable test mode"));
  Serial.println(Bell.armed()?F("4: Disarm system"):F("4: Arm system"));
  Serial.println(F("p<pin> Set pin"));
  Serial.println(F(""));
  if(Bell.testMode())
    Serial.println(F("*** TEST MODE ENABLED ***"));
  if(Bell.armed())
    Serial.println(F("*** SYSTEM IS ARMED ***"));
  Serial.print(F("Menu: "));
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
  Bell.setStrobe(!Bell.strobe());
}

void SerialMenuClass::toggleAlarm()
{
  Bell.setBell(!Bell.bell());
}

void SerialMenuClass::toggleTestMode()
{
  Bell.setTestMode(!Bell.testMode());
}

void SerialMenuClass::toggleArmed()
{
  Bell.setArmed(!Bell.armed());
}

void SerialMenuClass::setPin(const char *pin)
{
  if(*pin > 0) 
  {
    Menu.setPin(pin); 
    Serial.print(F("Pin reset to "));
    Serial.println(pin); 
  }
}

