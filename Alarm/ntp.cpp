#include "Arduino.h"
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Time.h>
#include "ntp.h"

#define DEBUG 0

NtpDriver Ntp;

#define LOCALPORT 65020
#define NTP_PACKET_SIZE 48 // NTP time stamp is in the first 48 bytes of the message

static byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

NtpDriver::NtpDriver()
{
}

void NtpDriver::sync(IPAddress address)
{
  _address = address;
  _responseTimeout = 10000;
  _lastPacketTime = 0;
  
#if DEBUG
  Serial.print("Sync with ");
  Serial.println(address);
#endif
  _udp.begin(LOCALPORT); 
  sendNtpPacket(address); // send an NTP packet to a time server
}

bool NtpDriver::maintain()
{
  if (_udp.parsePacket())
  {
#if DEBUG
    Serial.println("NTP Reply received");
#endif
    receiveNtpPacket();
    return true;
  }

  if((_lastPacketTime != 0) && (millis() > _lastPacketTime + _responseTimeout))
  {
#if DEBUG
    Serial.println("NTP response timeout - retrying");
#endif
    sync(_address);
  }
  return false;
}

// send an NTP request to the time server at the given address
void NtpDriver::sendNtpPacket(IPAddress address) 
{
  _lastPacketTime = millis();
  
#if DEBUG
  Serial.println("Send NTP packet");
#endif
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  
  if(!_udp.beginPacket(address, 123)) //NTP requests are to port 123
  {
#if DEBUG
    Serial.println("Unable to send ntp packet");
#endif
    return;
  }

  if(!_udp.write(packetBuffer, NTP_PACKET_SIZE))
  {
#if DEBUG
    Serial.println("Unable to send ntp packet");
#endif
  }

  if(!_udp.endPacket())
  {
#if DEBUG
    Serial.println("Unable to send ntp packet");
#endif
  }

#if DEBUG
  Serial.println("Sent NTP packet");
#endif
}

void NtpDriver::receiveNtpPacket()
{
  // We've received a packet, read the data from it
  _udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

  _lastPacketTime = 0;
  
  // the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, extract the two words:

  unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
  unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;

  // now convert NTP time into everyday time:
  // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
  const unsigned long seventyYears = 2208988800UL;
  // subtract seventy years:
  unsigned long epoch = secsSince1900 - seventyYears;

#if DEBUG
  // print Unix time:
  Serial.print("Unix time = ");
  Serial.println(epoch);

  Serial.print("Seconds since Jan 1 1900 = ");
  Serial.println(secsSince1900);

  // print the hour, minute and second:
  Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
  Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
  Serial.print(':');
  if (((epoch % 3600) / 60) < 10) {
    // In the first 10 minutes of each hour, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
  Serial.print(':');
  if ((epoch % 60) < 10) {
    // In the first 10 seconds of each minute, we'll want a leading '0'
    Serial.print('0');
  }
  Serial.println(epoch % 60); // print the second
#endif

  setTime(epoch);
}

