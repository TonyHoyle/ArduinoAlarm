#include "Arduino.h"
#include "pins.h"
#include "sensors.h"

SensorDriver Sensors;

void SensorDriver::begin()
{
  pinMode(SENSOR0, INPUT_PULLUP);
  pinMode(SENSOR1, INPUT_PULLUP);
  pinMode(SENSOR2, INPUT_PULLUP);
  pinMode(SENSOR3, INPUT_PULLUP);

  pinMode(SENSOR_TAMPER, INPUT_PULLUP);
  pinMode(KEYPAD_TAMPER, INPUT_PULLUP);
  pinMode(BELL_TAMPER, INPUT_PULLUP);

  pinMode(BELL_TRIGGER, OUTPUT);
  pinMode(STROBE, OUTPUT);
  
  digitalWrite(BELL_TRIGGER, HIGH);
  digitalWrite(STROBE, HIGH);

  maintain();

  // Start by assuming all working sensors are off
  _sensorMask = ~_sensor;
  _tamperMask = ~_tamper;
  
}

void SensorDriver::maintain()
{
  _sensor |= digitalRead(SENSOR0) | (digitalRead(SENSOR1)<<1) | (digitalRead(SENSOR2)<<2) | (digitalRead(SENSOR3)<<3);
  _tamper |= digitalRead(SENSOR_TAMPER) | (digitalRead(KEYPAD_TAMPER)<<1) | (digitalRead(BELL_TAMPER)<<2);

  // if any bit has gone to zero, it means it's active so reset its bit in the mask
  _sensorMask |= ~_sensor;
  _tamperMask |= ~_tamper;
}

void SensorDriver::bell(bool trigger)
{
  digitalWrite(BELL_TRIGGER, trigger?LOW:HIGH);
}

void SensorDriver::strobe(bool trigger)
{
  digitalWrite(STROBE, trigger?LOW:HIGH);
}


