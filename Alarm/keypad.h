#ifndef KEYPAD__H
#define KEYPAD__H

#include "melody.h"
#include "menu.h"

class KeypadDriver
{
  private:
    Melody _melody;
    Menu _menu;
    
    void send_message(int code, int count, const int *len, const void **msg);
    void incoming_key();
  public:
    KeypadDriver();

    void init();
    void clear();
    void message(const char *msg);
    void led(int zones, int leds);
    void beep(const uint16_t *notes);

    void maintain();

    void (*KeyHandler)(int key);
};

extern KeypadDriver Keypad;

#endif

