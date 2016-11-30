#ifndef BELL__H
#define BELL__H

#include "sensors.h"

class BellClass
{
  private:
    bool _bell;
    bool _strobe;
    bool _testMode;
    bool _armed;
    int _arming;

  public:
    void begin();
    void maintain();

    void pinEntered();

    inline bool bell() { return _bell; }
    inline bool strobe() { return _strobe; }
    inline bool testMode() { return _testMode; }
    inline bool armed() { return _armed; }
    inline void setBell(bool bell) { _bell = bell; Sensors.bell((!_testMode) && _bell); }
    inline void setStrobe(bool strobe) { _strobe = strobe; Sensors.strobe(_strobe); }
    inline void setTestMode(bool test) { _testMode = test; }
    inline void setArmed(bool armed) { _armed = armed; }
};

extern BellClass Bell;

#endif
