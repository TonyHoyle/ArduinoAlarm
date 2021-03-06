#ifndef SERIALMENu__H
#define SERIALMENU__H

class SerialMenuClass
{
  private:
    int freeRam();
    const char *getTime();

    void toggleStrobe();
    void toggleAlarm();
    void toggleTestMode();
    void toggleArmed();
    void setPin(const char *pin);

    void test5();
    void test6();

  public:
    void begin();
    void maintain();
    void showMenu();
};

extern SerialMenuClass SerialMenu;

#endif
