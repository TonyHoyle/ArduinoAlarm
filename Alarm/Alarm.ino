#include <Arduino.h>

#include <Ethernet.h>
#include <Time.h>

#include "pins.h"
#include "keypad.h"
#include "melody.h"
#include "tunelib.h"
#include "ntp.h"
#include "sensors.h"
#include "menu.h"
#include "serialmenu.h"
#include "bell.h"
#include "notify.h"

static byte mac[] = { 0xee, 0xb3, 0xf7, 0x59, 0x78, 0xde }; 
static IPAddress timeServer(130, 88, 200, 4); // ntp1.mcc.ac.uk

enum state_t { STARTING, RUNNING };

static state_t state;
static unsigned long ntpAfter;
static unsigned long lastRun;

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
  Melody.begin(KEYPAD_BEEP);
  Keypad.begin();
  Menu.begin();
  SerialMenu.begin();
  Bell.begin();
  Notify.begin();

//  Melody.play(Tunes.bbc_start, false);

  Keypad.clear();
  Keypad.message("BBC Computer 32K\nAcorn DFS");

  lastRun = 0;
  ntpAfter = 0;  
}

void loop() 
{
  unsigned long run = millis();
  
  if(run != lastRun)
  {
    // put your main code here, to run repeatedly:
    Melody.maintain();
    Menu.maintain();
    Keypad.maintain();
    Sensors.maintain();
    SerialMenu.maintain();
    Bell.maintain();
  
    if(state == RUNNING)
    {
      if(Ntp.maintain())
        SerialMenu.showMenu();

      if(run > ntpAfter)
      {
        Ntp.sync(timeServer);
        ntpAfter = run + (1000L * 60 * 60 + 24);
      }

      Notify.maintain();
    }
        
    if(Ethernet.maintain())
    {
      SerialMenu.showMenu();
      ntpAfter = run + 100L;   
      state = RUNNING;
    }

    lastRun = run;
  }
}


