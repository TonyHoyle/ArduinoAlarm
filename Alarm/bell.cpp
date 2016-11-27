#include "Arduino.h"
#include "sensors.h"
#include "bell.h"

BellClass Bell;

void BellClass::begin()
{
  _bell = false;
  _strobe = false;
  _testMode = false;
}

void BellClass::maintain()
{
  if(_armed) {
    int sensor = Sensors.sensor();
    int tamper = Sensors.tamper();

    if(sensor || tamper) {
      Serial.print(F("** Alarm triggered "));
      Serial.print(sensor|512, BIN);
      Serial.print(F(" "));
      Serial.print(tamper|512, BIN);
      Serial.println(F(" **"));  

//      setStrobe(true);
//      setAlarm(true);
    }
  }
}

