#include "Arduino.h"
#include "sensors.h"
#include "melody.h"
#include "tunelib.h"
#include "pins.h"
#include "notify.h"
#include "bell.h"

BellClass Bell;

#define DEBUG 0

void BellClass::begin()
{
  _bell = false;
  _strobe = false;
  _testMode = false;
  _armTime = 0;
  _arming = DAY_MODE;
}

void BellClass::maintain()
{
  int sensor = Sensors.sensor();
  int tamper = Sensors.tamper();

  if(sensor || tamper)
    Notify.motion(sensor, tamper);
    
  if (_arming == ARMED)
  {
    if (sensor || tamper)
    {
      if ((sensor & EXIT_SENSORS) && !tamper)
      {
#if DEBUG
        Serial.println(F("Entry triggered.  30 seconds until alarm."));
#endif
        _arming = ENTRY;
        _armTime = millis() + (30 * 1000L);
        Melody.play(Tunes.warning_beep, true);
      }
      else
        trigger(sensor, tamper);
    }
  }
  else if (_arming == ENTRY)
  {
    if (_armTime <= millis())
      trigger(sensor, tamper);
  } else if (_arming == WAIT_FOR_EXIT_OPEN)
  {
    sensor = Sensors.liveSensor() & EXIT_SENSORS;
    if (sensor)
    {
      _arming = WAIT_FOR_EXIT_CLOSE;
#if DEBUG
      Serial.println(F("Door open.  Waiting for door close."));
#endif
    }
  } else if (_arming == WAIT_FOR_EXIT_CLOSE)
  {
    sensor = Sensors.liveSensor() & EXIT_SENSORS;
    if (!sensor)
    {
#if DEBUG
      Serial.println(F("Door closed.  Arming system in 10 seconds."));
#endif
      _arming = PRE_ARM;
      _armTime = millis() + (10 * 1000L);
      Melody.play(Tunes.arming_beep, false);
    }
  }
  else if (_arming == PRE_ARM)
  {
    if (_armTime <= millis())
    {
#if DEBUG
      Serial.println(F("System armed."));
#endif
      _arming = ARMED;
      Melody.stop(); /* Just in case */
      Notify.armed(true);
    }
  }
}

void BellClass::pinEntered()
{
  if (_arming != DAY_MODE)
  {
    setStrobe(false);
    setBell(false);
    _arming = DAY_MODE;
    Melody.stop();
    Notify.armed(false);
  }
  else
  {
    Melody.play(Tunes.warning_beep, true);
    _arming = WAIT_FOR_EXIT_OPEN;
  }
}

void BellClass::trigger(int sensor, int tamper)
{
#if DEBUG
  Serial.print(F("** Alarm triggered "));
  Serial.print(sensor | 512, BIN);
  Serial.print(F(" "));
  Serial.print(tamper | 512, BIN);
  Serial.println(F(" **"));
#endif
  setStrobe(true);
  //      setAlarm(true);
  Notify.triggered();
}

void BellClass::arm(bool armed)
{
  if (!armed)
  {
    setStrobe(false);
    setBell(false);
    _arming = DAY_MODE;
    Notify.armed(false);
  }
  else
  {
    _arming = ARMED;
    Notify.armed(true);
  }
}

