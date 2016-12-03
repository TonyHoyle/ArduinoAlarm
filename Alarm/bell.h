#ifndef BELL__H
#define BELL__H

#include "sensors.h"

class BellClass
{
  private:
    enum bellState_t { DAY_MODE, WAIT_FOR_EXIT_OPEN, WAIT_FOR_EXIT_CLOSE, PRE_ARM, ENTRY, ARMED };
    
    bool _bell;
    bool _strobe;
    bool _testMode;
    bellState_t _arming;
    unsigned long _armTime;

    void trigger(int sensor, int tamper);
    

  public:
    void begin();
    void maintain();

    void pinEntered();
    void arm(bool armed);

    inline bool bell() { return _bell; }
    inline bool strobe() { return _strobe; }
    inline bool testMode() { return _testMode; }
    inline bool armed() { return _arming==ARMED; }
    inline void setBell(bool bell) { _bell = bell; Sensors.bell((!_testMode) && _bell); }
    inline void setStrobe(bool strobe) { _strobe = strobe; Sensors.strobe(_strobe); }
    inline void setTestMode(bool test) { _testMode = test; }
};

extern BellClass Bell;

#endif
