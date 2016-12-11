#include "Arduino.h"
#include "melody.h"

MelodyClass Melody;

#define DEBUG 0

static const int noteGap = 50;

/* Control tone() to generate a melody.  Passed an array of note, duration terminated by 0,0 */

void MelodyClass::begin(int pin)
{
  _pin = pin;
  _nextTone = NULL;
}

void MelodyClass::play(const uint16_t *tones, bool loop)
{
#if DEBUG
      Serial.print("Play ");
      Serial.println(int(tones), HEX);
#endif
  _nextInterval = 0; /* Start on next call to loop() - this may not be immediate */
  _nextTone = tones;
  _gap = false;
  if (loop) _loopPoint = tones;
  else _loopPoint = NULL;
}

void MelodyClass::beep(uint16_t note, uint16_t duration)
{
#if DEBUG
      Serial.print("Beep ");
      Serial.print(note);
      Serial.print(",");
      Serial.println(duration);
#endif
  _nextInterval = millis() + duration; // Interrupt current song
  _gap = true;
  tone(_pin, note);
}

void MelodyClass::stop()
{
  _loopPoint = NULL;
  _nextTone = NULL;
  noTone(_pin);
}

void MelodyClass::maintain()
{
  unsigned int mil = millis();
  
  if (mil < _nextInterval) return;
  if (_nextTone == NULL) 
  {
    noTone(_pin);
    return;
  }

  /* Special case for startup */
  if (_nextInterval == 0) _nextInterval = mil;

  uint16_t note = pgm_read_word(_nextTone);
  uint16_t duration = pgm_read_word(_nextTone + 1);
  if (!_gap)
  {
#if DEBUG
    Serial.println("Note: " + String(note) + "," + String(duration));
#endif
    if (note == 0) /* delay */
      noTone(_pin);
    else
      tone(_pin, note);
    _nextInterval = _nextInterval + duration;
    _gap = true;
  }
  else
  {
#if DEBUG
    Serial.println("Gap: " + String(note) + "," + String(duration));
#endif
    noTone(_pin);
    _nextInterval = _nextInterval + noteGap;
    _nextTone += 2;
    _gap = false;

    if (duration == 0) /* end */
    {
#if DEBUG
      Serial.print("Loop to ");
      Serial.println(int(_loopPoint), HEX);
#endif
      _nextTone = _loopPoint;
    }
  }
}

