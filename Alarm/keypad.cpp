#include "Arduino.h"
#include "pins.h"
#include "menu.h"
#include "keypad.h"

#define DEBUG 0

KeypadClass Keypad;

void KeypadClass::begin()
{
  Serial1.begin(1200, SERIAL_9N1);
}

void KeypadClass::clear()
{
  message("\xc");
}

void KeypadClass::message(const char *msg)
{
  int len = strlen(msg);
  int l[2] = { 1, len };
  const void *m[2] = { &len, msg };

#if DEBUG
  Serial.println("Message:");
  Serial.println(msg);
#endif

  send_message('L', 2, l, m);
}

void KeypadClass::led(int zones, int leds)
{
  const unsigned char msg[] = { (unsigned char)zones, (unsigned char)leds };
  int l[1] = { 2 };
  const void *m[1] = { msg };

  send_message('P', 1, l, m);
}

void KeypadClass::send_message(int code, int count, const int *len, const void **msg)
{
  Serial1.write(code + 256);
  int chk = code;
  for (int j = 0; j < count; j++)
  {
    for (int n = 0; n < len[j]; n++)
    {
      chk = chk + ((unsigned char *)msg[j])[n];
      Serial1.write(((unsigned char *)msg[j])[n]);
    }
  }
  Serial1.write(chk & 255);
}

void KeypadClass::maintain()
{
  int byte;

  if (!Serial1.available())
    return;

  while (Serial1.available())
  {
    byte = Serial1.read();
#if DEBUG
    Serial.print("Byte: ");
    Serial.print(byte, HEX);
    Serial.println("");
#endif
    if (byte & 256) break;
  }
  if (!(byte & 256)) return;

#if DEBUG
  Serial.print("Command: ");
  Serial.write(byte);
  Serial.println("");
#endif
  switch (byte & 255)
  {
    case 'K': // Key
      incoming_key();
      break;
  }
}


void KeypadClass::incoming_key()
{
  while (!Serial1.available()) ;
  int key = Serial1.read();
  while (!Serial1.available()) ;

#if DEBUG
  int chk = Serial1.read();
  Serial.print("Key: ");
  Serial.write(key);
  if (chk != ('K' + key) & 255)
  {
    Serial.print("Checksum failed: ");
    Serial.print(chk);
    Serial.print(" ");
    Serial.print(('K' + key) & 255);
  }
  Serial.println("");
#else
  Serial1.read();
#endif

  tone(KEYPAD_BEEP, 1046, 100);

  Menu.keyHandler((key&255));
}



