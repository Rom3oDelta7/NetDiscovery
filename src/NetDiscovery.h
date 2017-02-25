/*
 Network Discovery Library - a simple discovery protocol using UDP multicast

 Copyright 2017 Rob Redford
 This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/4.0/.

*/

#ifndef NETDISCOVERY_H
#define NETDISCOVERY_H

#if defined(ARDUINO) && (ARDUINO >= 10605)
#include "Arduino.h"
#else
#error This library requires Arduino IDE 1.6.5 or above
#endif

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define ND_DEBUG 3

#ifdef ND_DEBUG
#define ND_DEBUG_MSG(L, H, M)	       if ((L) <= ND_DEBUG) {Serial.print("ND_DEBUG> "); Serial.print(H); Serial.print(": "); Serial.println(M);}
#else
#define ND_DEBUG_MSG(...)            ;
#endif

#define ND_MCAST_PORT_LOW    4097          // lowest valid port number
#define ND_MCAST_PORT_HIGH   49151         // highest valid port number

class NetDiscovery {
public:
	// user functions
	bool begin (const IPAddress localIP, const IPAddress multicastIP, const int mcastPort);
	bool listen (const IPAddress localIP, void *packet, const size_t packetSize, const void *ackPacket, const size_t ackPacketSize);
	bool announce (const IPAddress localIP, const void *packet, const size_t packetSize, void *ackPacket, const size_t ackPacketSize);

private:
	WiFiUDP   _mcast;	                 // multicast UDP stream
	IPAddress _mcastIP;                // multicast group address 239.X.Y.Z
	int       _mcastPort;              // multicast port number

	inline bool _inRange (const int octet);

};
#endif