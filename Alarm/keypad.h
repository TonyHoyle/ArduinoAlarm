#ifndef KEYPAD__H
#define KEYPAD__H

class KeypadClass
{
  private:
    void send_message(int code, int count, const int *len, const void **msg);
    void incoming_key();
  public:
    void begin();
    void maintain();

    void clear();
    void message(const char *msg);
    void led(int zones, int leds);
};

extern KeypadClass Keypad;

#endif

