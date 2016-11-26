#ifndef NTP__H
#define NTP__H

class NtpDriver
{
  private:
    EthernetUDP _udp;
    IPAddress _address;
    int _responseTimeout;
    int _lastPacketTime;

    void sendNtpPacket(IPAddress address); 
    void receiveNtpPacket();

  public:
    NtpDriver();

    void sync(IPAddress address);
    bool maintain();
};

extern NtpDriver Ntp;

#endif
