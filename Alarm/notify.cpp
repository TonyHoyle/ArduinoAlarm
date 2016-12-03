#include "Arduino.h"
#include <PubSubClient.h>
#include <Ethernet.h>
#include "Bell.h"
#include "Notify.h"

NotifyClass Notify;

#define DEBUG 0

static const char server[] = "alarm";
static const char user[] = "alarm";
static const char pass[] = "gnurf99";

static const char version[] = "1.0.0";

void NotifyClass::begin()
{
  _lastReconnectAttempt = 0;
  _lastMotionMessage = 0;

  _client.setClient(_ethClient);
  _client.setServer("candy.hoyle.me.uk", 1883);
  _client.setCallback(callback);
}

void NotifyClass::maintain()
{
  unsigned long run = millis();
  if (!_client.connected())
  {
    if ((run - _lastReconnectAttempt) > 5000)
    {
      _lastReconnectAttempt = 5000L;
      if (reconnect()) _lastReconnectAttempt = 0;
    }
  }
  else
    _client.loop();
}

bool NotifyClass::reconnect()
{
  if (_client.connect(server, user, pass)) {
    // Once connected, publish an announcement...
#if DEBUG
    Serial.println(F("Connected to mqtt"));
#endif
    _client.publish("alarm/version", version, true);
    _client.subscribe("alarm/arm");
    armed(Bell.armed());
  }
  return _client.connected();
}

void NotifyClass::callback(char* topic, byte* payload, unsigned int length)
{
  if (!strcmp(topic, "alarm/arm") && length == 1)
  {
#if DEBUG
    Serial.print(F("Remote set armed "));
    Serial.write(payload[0]);
    Serial.println();
#endif
    if (payload[0] == '0') Bell.arm(false);
    else if (payload[0] == '1') Bell.arm(true);
  }
  else
  {
#if DEBUG
    Serial.print(F("Unknown Message arrived ["));
    Serial.print(topic);
    Serial.print(F("] "));
    Serial.print(length);
    Serial.print(F(": "));
    for (int i = 0; i < (int)length; i++)
      Serial.print((char)payload[i]);
    Serial.println();
#endif
  }
}

void NotifyClass::armed(bool armed)
{
  _client.publish("alarm/armed", armed ? "1" : "0", true);
}

void NotifyClass::motion(int sensor, int tamper)
{
  static char buf[10];

  if (!sensor && !tamper)
    return;

  if ((millis() - _lastMotionMessage) < 1000)
    return; // rate limit

  _lastMotionMessage = millis();

#if DEBUG
  Serial.print("Motion ");
  Serial.print(sensor);
  Serial.print(" ");
  Serial.print(tamper);
  Serial.print(" ");
  Serial.println(millis());
#endif

  if (sensor)
    _client.publish("alarm/motion", itoa(sensor, buf, 10));
  if (tamper)
    _client.publish("alarm/tamper", itoa(tamper, buf, 10));
}

void NotifyClass::triggered()
{
  _client.publish("alarm/triggered", "1");
}

