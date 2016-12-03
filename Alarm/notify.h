#ifndef NOTIFY__H
#define NOTIFY__H

#include <Ethernet.h>
#include <PubSubClient.h>

class NotifyClass
{
  private:
    unsigned long _lastReconnectAttempt;
    EthernetClient _ethClient;
    PubSubClient _client;
    unsigned long _lastMotionMessage;

    bool reconnect(); 
    static void callback(char* topic, byte* payload, unsigned int length);

  public:
    void begin();
    void maintain();

    void armed(bool armed);
    void motion(int sensor, int tamper);
    void triggered();
};

extern NotifyClass Notify;

#endif
