#ifndef TUNELIB__H
#define TUNETLIB__H

typedef struct
{
  const uint16_t *bbc_start;
  const uint16_t *mario;
} tunes_t;

extern const tunes_t Tunes;

#endif