#include "Arduino.h"
#include "keypad.h"
#include "pins.h"

#define DEBUG 0

KeypadDriver Keypad;

KeypadDriver::KeypadDriver() : _melody(KEYPAD_BEEP)
{
  KeyHandler = NULL;
}

void KeypadDriver::init()
{
  Serial1.begin(1200, SERIAL_9N1);
}

void KeypadDriver::clear()
{
  message("\xc");
}

void KeypadDriver::beep(const uint16_t *notes)
{
  _melody.play(notes);
}

void KeypadDriver::message(const char *msg)
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

void KeypadDriver::led(int zones, int leds)
{
  const unsigned char msg[] = { (unsigned char)zones, (unsigned char)leds };
  int l[1] = { 2 };
  const void *m[1] = { msg };

  send_message('P', 1, l, m);
}

void KeypadDriver::send_message(int code, int count, const int *len, const void **msg)
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

void KeypadDriver::maintain()
{
  int byte;

  _melody.maintain();
  _menu.maintain();

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


void KeypadDriver::incoming_key()
{
  while (!Serial1.available()) ;
  int key = Serial1.read();
  while (!Serial1.available()) ;
  int chk = Serial1.read();

#if DEBUG
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
#endif

  tone(KEYPAD_BEEP, 1046, 100);

  _menu.keyHandler((key&255));
}



