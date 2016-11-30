#ifndef MELODY__H
#define MELODY__H

class MelodyClass
{
  private:
    const uint16_t *_nextTone;
    unsigned long _nextInterval;
    int _pin;
    bool _gap;
    const uint16_t *_loopPoint;
    
  public:
    void begin(int pin);
    void maintain();

    void play(const uint16_t *notes, bool loop);
    void stop();
};

extern MelodyClass Melody;

#endif
