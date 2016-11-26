#ifndef SERIALMENu__H
#define SERIALMENU__H

class SerialMenuClass
{
  private:
    bool _alarm;
    bool _strobe;
    
    int freeRam();
    const char *getTime();

    void toggleStrobe();
    void toggleAlarm();

  public:
    void begin();
    void maintain();
    void showMenu();
};

extern SerialMenuClass SerialMenu;

#endif
