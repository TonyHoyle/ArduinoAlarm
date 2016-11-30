#include "Arduino.h"
#include "sensors.h"
#include "melody.h"
#include "tunelib.h"
#include "pins.h"
#include "bell.h"

BellClass Bell;

#define DEBUG 1

enum { NOT_ARMING, WAIT_FOR_EXIT_OPEN, WAIT_FOR_EXIT_CLOSE };

void BellClass::begin()
{
  _bell = false;
  _strobe = false;
  _testMode = false;
  _arming = NOT_ARMING;
}

void BellClass::maintain()
{
  if(_armed) {
    int sensor = Sensors.sensor();
    int tamper = Sensors.tamper();

    if(sensor || tamper) {
#if DEBUG 
      Serial.print(F("** Alarm triggered "));
      Serial.print(sensor|512, BIN);
      Serial.print(F(" "));
      Serial.print(tamper|512, BIN);
      Serial.println(F(" **"));  
#endif

      setStrobe(true);
//      setAlarm(true);
    }
  } else if(_arming == WAIT_FOR_EXIT_OPEN)
  {
    int sensor = Sensors.liveSensor() && EXIT_SENSORS;
    if(sensor) 
    {
      _arming = WAIT_FOR_EXIT_CLOSE;
#if DEBUG
      Serial.println(F("Door open.  Waiting for door close."));
#endif       
    }
  } else if(_arming == WAIT_FOR_EXIT_CLOSE)
  {
    int sensor = Sensors.liveSensor() && EXIT_SENSORS;
    if(!sensor) 
    {
#if DEBUG
      Serial.println(F("Door closed.  Arming system."));
#endif       
      _arming = NOT_ARMING;
      _armed = true;
      Melody.stop();
    }
  }
}

void BellClass::pinEntered()
{
  if(_armed)
  {
    setStrobe(false);
    setBell(false);
    _armed = false;
    _arming = NOT_ARMING;
    Melody.stop();
  }
  else
  {
    Melody.play(Tunes.warning_beep, true);
    _arming = WAIT_FOR_EXIT_OPEN;    
  }
}

