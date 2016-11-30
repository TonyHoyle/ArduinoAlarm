#include <TimeLib.h>
#include <Ethernet.h>
#include <EEPROM.h>

#include "Arduino.h"
#include "keypad.h"
#include "tunelib.h"
#include "melody.h"
#include "bell.h"
#include "menu.h"

#define PINMODE -1
#define MENUMODE -2

MenuClass Menu;

const menu_t MenuClass::_menu[] =
{
  { '1', "Tunes", &MenuClass::tunesMenu },
  { '2', "Network", &MenuClass::networkMenu },
  { 0 }
};


void MenuClass::begin()
{
  _currentMenu = PINMODE;
  EEPROM.get(0, _data);
  if(_data.size != sizeof(_data) || _data.version != 1)
    setPin("1111");

  _keyCount = 0;
  _lastMinute = -1;
}

void MenuClass::keyHandler(char key)
{
  if(Bell.armed()) _currentMenu = PINMODE;
  
  if (_currentMenu == PINMODE) pinKey(key);
  else if (_currentMenu == MENUMODE) menuKey(key);
  else
  {
    if (key == '?')
    {
      _currentMenu = MENUMODE;
      updateMessage();
      return;
    }

    (this->*_menu[_currentMenu].function)(key);
  }
}

void MenuClass::pinKey(char key)
{
  if (key == '=')
  {
    _currentMenu = MENUMODE;
    _keyCount = 0;
    updateMessage();
    return;
  }

  if (key < '0' || key > '9') return;

  _enteredPin[_keyCount++] = (char)key;
  if (_keyCount == _data.pinLength)
  {
    _keyCount = 0;
    if (!memcmp(_data.pin, _enteredPin, _data.pinLength))
    {
      Bell.pinEntered();
      updateMessage();
    }
  }
}

void MenuClass::menuKey(char key)
{
  if (key == '?')
  {
    _currentMenu = PINMODE;
    updateMessage();
    return;
  }

  for (int i = 0; _menu[i].key; ++i)
  {
    if (_menu[i].key == key)
    {
      _currentMenu = i;
      updateMessage();
      return;
    }
  }
}

void MenuClass::updateMessage()
{
  Keypad.clear();
  if(Bell.armed())
    Keypad.message("Alarm Set");
  else if (_currentMenu == PINMODE)
    Keypad.message(getTime());
  else if (_currentMenu == MENUMODE)
    Keypad.message("Menu Mode");
  else
    Keypad.message(_menu[_currentMenu].prompt);
}

const char *MenuClass::getTime()
{
  static char s_time[64];
  static char s_day[4];

  strncpy(s_day, dayShortStr(weekday()), sizeof(s_day));
  snprintf(s_time, sizeof(s_time), "%s %02d %s %04d\n    %2d:%02d %s", s_day, day(), monthShortStr(month()), year(), hourFormat12(), minute(), isAM() ? "AM" : "PM");
  return s_time;
}

void MenuClass::tunesMenu(char key)
{
  switch (key)
  {
    case '1': Melody.play(Tunes.mario, false); break;
    case '2': Melody.play(Tunes.bbc_start, false); break;
  }
}

void MenuClass::networkMenu(char key)
{
  IPAddress localIP;
  char ip[17];

  switch (key)
  {
    case '1':
      localIP = Ethernet.localIP();
      snprintf(ip, sizeof(ip), "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);
      Keypad.message(ip);
      break;
  }
}

void MenuClass::maintain()
{
  int min = minute();
  if(min != _lastMinute)
    updateMessage();
  _lastMinute = min;
}

void MenuClass::update()
{
  updateMessage();
}

void MenuClass::setPin(const char *pin)
{
  _data.pinLength = min(MAX_PIN_LENGTH, strlen(pin));
  memcpy(_data.pin, pin, _data.pinLength);
  
  _data.size = sizeof(_data);
  _data.version = 1;
  EEPROM.put(0, _data);
}

const char *MenuClass::getPin()
{
  return _data.pin;
}

byte MenuClass::getPinLength()
{
  return _data.pinLength;
}


