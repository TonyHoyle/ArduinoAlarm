#ifndef STATE__H
#define STATE__H

#define MAX_PIN_LENGTH 12

class MenuClass;

struct menu_t { char key; const char *prompt; void(MenuClass::*function)(char key); };

class MenuClass
{
  private:
    struct eepromData_t
    {
      byte size;
      byte version;
      byte pinLength;
      char pin[MAX_PIN_LENGTH];
    } _data;
    
    int _currentMenu;
    char _enteredPin[MAX_PIN_LENGTH];
    int _keyCount;
    static const menu_t _menu[];
    int _lastMinute;
    
    void pinKey(char key);
    void menuKey(char key);
    void updateMessage();
    const char *getTime();

    void tunesMenu(char key);
    void networkMenu(char key);

  public:
    void begin();
    void maintain();

    void keyHandler(char key);
    void update();

    void setPin(const char *pin);
    const char *getPin();
    byte getPinLength();
};

extern MenuClass Menu;

#endif
