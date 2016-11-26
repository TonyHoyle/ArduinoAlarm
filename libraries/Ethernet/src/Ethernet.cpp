#include "utility/w5100.h"
#include "Ethernet.h"
#include "Dhcp.h"

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
uint8_t EthernetClass::_state[MAX_SOCK_NUM] = { 
  0, 0, 0, 0 };
uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = { 
  0, 0, 0, 0 };

static DhcpClass s_dhcp;

int EthernetClass::begin(uint8_t *mac_address, const char *hostName, unsigned long responseTimeout)
{
  _dhcp = &s_dhcp;

  // Initialise the basic info
  W5100.init();
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5100.setMACAddress(mac_address);
  W5100.setIPAddress(IPAddress(0,0,0,0).raw_address());
  SPI.endTransaction();
  
  // Now try to get our config info from a DHCP server
  int ret = _dhcp->beginWithDHCP(mac_address, hostName, responseTimeout);

  return ret;
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip)
{
  // Assume the DNS server will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress dns_server = local_ip;
  dns_server[3] = 1;
  begin(mac_address, local_ip, dns_server);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server)
{
  // Assume the gateway will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress gateway = local_ip;
  gateway[3] = 1;
  begin(mac_address, local_ip, dns_server, gateway);
}

void EthernetClass::begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
{
  IPAddress subnet(255, 255, 255, 0);
  begin(mac_address, local_ip, dns_server, gateway, subnet);
}

void EthernetClass::begin(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
  W5100.init();
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5100.setMACAddress(mac);
  W5100.setIPAddress(local_ip.raw_address());
  W5100.setGatewayIp(gateway.raw_address());
  W5100.setSubnetMask(subnet.raw_address());
  SPI.endTransaction();
  _dnsServerAddress = dns_server;
}

int EthernetClass::restart(uint8_t *mac_address, const char *hostName, unsigned long responseTimeout)
{
  _dhcp = &s_dhcp;
  return _dhcp->beginWithDHCP(mac_address, hostName, responseTimeout);
}

bool EthernetClass::maintain()
{
  int rc = DHCP_CHECK_NONE;
  if(_dhcp != NULL) 
  {
	  if(_dhcp->checkLease())
	  {
		SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
		W5100.setIPAddress(_dhcp->getLocalIp().raw_address());
		W5100.setGatewayIp(_dhcp->getGatewayIp().raw_address());
		W5100.setSubnetMask(_dhcp->getSubnetMask().raw_address());
		SPI.endTransaction();
		_dnsServerAddress = _dhcp->getDnsServerIp();
		
		return true;
	  }		
  }
  return rc;
}

IPAddress EthernetClass::localIP()
{
  IPAddress ret;
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5100.getIPAddress(ret.raw_address());
  SPI.endTransaction();
  return ret;
}

IPAddress EthernetClass::subnetMask()
{
  IPAddress ret;
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5100.getSubnetMask(ret.raw_address());
  SPI.endTransaction();
  return ret;
}

IPAddress EthernetClass::gatewayIP()
{
  IPAddress ret;
  SPI.beginTransaction(SPI_ETHERNET_SETTINGS);
  W5100.getGatewayIp(ret.raw_address());
  SPI.endTransaction();
  return ret;
}

IPAddress EthernetClass::dnsServerIP()
{
  return _dnsServerAddress;
}

EthernetClass Ethernet;