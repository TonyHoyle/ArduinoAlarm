#include <Ethernet.h>
#include <Time.h>

#include "pins.h"
#include "keypad.h"
#include "melody.h"
#include "tunelib.h"
#include "ntp.h"
#include "sensors.h"
#include "serialmenu.h"

static byte mac[] = { 0xee, 0xb3, 0xf7, 0x59, 0x78, 0xde }; 
static IPAddress timeServer(130, 88, 200, 4); // ntp1.mcc.ac.uk

enum state_t { STARTING, RUNNING };
static state_t state;
unsigned long ntpAfter;

static unsigned long lastMil;

void setup() {
  // put your setup code here, to run once:
  Sensors.begin();

  pinMode(KEYPAD_BEEP, OUTPUT);
  pinMode(EXT_SPEAKER, OUTPUT);

  // disable SD card
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  state = STARTING;

  Serial.begin(9600);
  
  Ethernet.begin(mac);

  Keypad.init();

  Keypad.beep(Tunes.bbc_start);

  Keypad.clear();
  Keypad.message("BBC Computer 32K\nAcorn DFS");

  lastMil = 0;
  ntpAfter = 0;

  SerialMenu.begin();
}

void loop() 
{
  unsigned long mil = millis();
  
  if(mil != lastMil)
  {
    // put your main code here, to run repeatedly:
    Keypad.maintain();
  
    if(state == RUNNING)
    {
      if(Ntp.maintain())
        SerialMenu.showMenu();

      if(mil > ntpAfter)
      {
        Ntp.sync(timeServer);
        ntpAfter = mil + (1000L * 60 * 60 + 24);
      }
    }
        
    if(Ethernet.maintain())
    {
      SerialMenu.showMenu();
      ntpAfter = mil + 2000L;   
      state = RUNNING;
    }

    Sensors.maintain();
    SerialMenu.maintain();

    lastMil = mil;
  }
}


