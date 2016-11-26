#include <TimeLib.h>
#include <Ethernet.h>
#include <EEPROM.h>

#include "Arduino.h"
#include "menu.h"
#include "keypad.h"
#include "tunelib.h"

#define PINMODE -1
#define MENUMODE -2

const menu_t Menu::_menu[] =
{
  { '1', "Tunes", &Menu::tunesMenu },
  { '2', "Network", &Menu::networkMenu },
  { 0 }
};

Menu::Menu()
{
  _currentMenu = PINMODE;
  EEPROM.get(0, _data);
  if(_data.size != sizeof(_data) || _data.version != 1)
  {
    _data.pinLength = 4;
    memcpy(_data.pin, "1111", _data.pinLength);
    _data.size = sizeof(_data);
    _data.version = 1;
    EEPROM.put(0, _data);
  }
  _keyCount = 0;
  _dayMode = true;
  _lastMinute = -1;
}

void Menu::keyHandler(char key)
{
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

void Menu::pinKey(char key)
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
      _dayMode = !_dayMode;
      updateMessage();
      if (!_dayMode)
      {
        // Perform exit, etc.
      }
      else
      {
        // Entered day mode
      }
    }
  }
}

void Menu::menuKey(char key)
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

void Menu::updateMessage()
{
  Keypad.clear();
  if (_currentMenu == PINMODE)
  {
    if (!_dayMode) Keypad.message("Alarm Set");
    else Keypad.message(getTime());
  }
  else if (_currentMenu == MENUMODE)
    Keypad.message("Menu Mode");
  else
    Keypad.message(_menu[_currentMenu].prompt);
}

const char *Menu::getTime()
{
  static char s_time[64];
  static char s_day[4];

  strncpy(s_day, dayShortStr(weekday()), sizeof(s_day));
  snprintf(s_time, sizeof(s_time), "%s %02d %s %04d\n    %2d:%02d %s", s_day, day(), monthShortStr(month()), year(), hourFormat12(), minute(), isAM() ? "AM" : "PM");
  return s_time;
}

void Menu::tunesMenu(char key)
{
  switch (key)
  {
    case '1': Keypad.beep(Tunes.mario); break;
    case '2': Keypad.beep(Tunes.bbc_start); break;
  }
}

void Menu::networkMenu(char key)
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

void Menu::maintain()
{
  int min = minute();
  if(min != _lastMinute)
    updateMessage();
  _lastMinute = min;
}

void Menu::update()
{
  updateMessage();
}


