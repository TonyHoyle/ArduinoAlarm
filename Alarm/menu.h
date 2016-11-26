#ifndef STATE__H
#define STATE__H

#define MAX_PIN_LENGTH 12

class Menu;

struct menu_t { char key; const char *prompt; void(Menu::*function)(char key); };

class Menu
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
    bool _dayMode;
    static const menu_t _menu[];
    int _lastMinute;
    
    void pinKey(char key);
    void menuKey(char key);
    void updateMessage();
    const char *getTime();

    void tunesMenu(char key);
    void networkMenu(char key);

  public:
    Menu();

    void keyHandler(char key);
    void init();
    void maintain();
    void update();
};

#endif
