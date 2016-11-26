#ifndef MELODY__H
#define MELODY__H

class Melody
{
  private:
    const uint16_t *_nextTone;
    unsigned long _nextInterval;
    int _pin;
    bool _gap;
    
  public:
    Melody(int pin);

    void play(const uint16_t *notes);
    void maintain();
};

#endif
