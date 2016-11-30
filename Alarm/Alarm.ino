#include <Arduino.h>

#include <Ethernet.h>
#include <Time.h>
#include <PubSubClient.h>

#include "pins.h"
#include "keypad.h"
#include "melody.h"
#include "tunelib.h"
#include "ntp.h"
#include "sensors.h"
#include "menu.h"
#include "serialmenu.h"
#include "bell.h"

static byte mac[] = { 0xee, 0xb3, 0xf7, 0x59, 0x78, 0xde }; 
static IPAddress timeServer(130, 88, 200, 4); // ntp1.mcc.ac.uk

enum state_t { STARTING, RUNNING };

static state_t state;
static unsigned long ntpAfter;
static unsigned long lastRun;
static unsigned long lastReconnectAttempt;

EthernetClient ethClient;
PubSubClient client(ethClient);

boolean reconnect() 
{
  if (client.connect("alarm", "alarm", "gnurf99")) {
    // Once connected, publish an announcement...
    client.publish("outTopic","hello world");
    // ... and resubscribe
    client.subscribe("inTopic");
  }
  return client.connected();
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<(int)length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

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

  Melody.play(Tunes.bbc_start, false);

  Keypad.clear();
  Keypad.message("BBC Computer 32K\nAcorn DFS");

  lastRun = 0;
  ntpAfter = 0;  
  lastReconnectAttempt = 0;

  client.setServer("candy.hoyle.me.uk", 1883);
  client.setCallback(callback);
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

      if(!client.connected()) {
        if((run - lastReconnectAttempt) > 5000) {
          lastReconnectAttempt = 5000L;
          if(reconnect()) lastReconnectAttempt = 0;
        }
      }
      else
        client.loop();
    }
        
    if(Ethernet.maintain())
    {
      SerialMenu.showMenu();
      ntpAfter = run + 2000L;   
      state = RUNNING;
    }

    lastRun = run;
  }
}


