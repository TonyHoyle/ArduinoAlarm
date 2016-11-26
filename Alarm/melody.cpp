#include "Arduino.h"
#include <avr/pgmspace.h>
#include "melody.h"

#define DEBUG 0

static const int noteGap = 50;

/* Control tone() to generate a melody.  Passed an array of note, duration terminated by 0,0 */

Melody::Melody(int pin)
{
  _pin = pin;
}

void Melody::play(const uint16_t *tones)
{
  _nextTone = tones;
  _nextInterval = 0; /* Start on next call to loop() - this may not be immediate */
  _gap = false;
}

void Melody::maintain()
{
  int mil = millis();
  if(_nextTone == NULL || mil < _nextInterval) return;

  /* Special case for startup */
  if(_nextInterval == 0) _nextInterval = mil;

  uint16_t note = pgm_read_word(_nextTone);
  uint16_t duration = pgm_read_word(_nextTone+1);
  if(!_gap)
  {
#if DEBUG
    Serial.println("Note: " + String(note) + "," + String(duration));
#endif
    if(note == 0) /* delay */
      noTone(_pin);
    else
      tone(_pin, note);
    _nextInterval = _nextInterval + duration;
    _gap = true;
  }
  else
  {
    noTone(_pin);
    _nextInterval = _nextInterval + noteGap;
    _nextTone += 2;
    _gap = false;

    if(duration == 0) /* end */
      _nextTone = NULL;    
  }
}

