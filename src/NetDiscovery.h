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

//#define ND_DEBUG 3                 // define as minimum desired debug level, or comment out to disable debug statements

#ifdef ND_DEBUG
#define ND_DEBUG_MSG(L, H, M)	       if ((L) <= ND_DEBUG) {Serial.print("ND_DEBUG> "); Serial.print(H); Serial.print(": "); Serial.println(M);}
#else
#define ND_DEBUG_MSG(...)            ;
#endif

#define ND_MCAST_PORT_LOW    4097          // lowest valid port number
#define ND_MCAST_PORT_HIGH   49151         // highest valid port number

// define discovery packet types
typedef enum {
	ND_ANNOUNCE = 0x11,
   ND_ACK = 0x22,
	ND_INFO = 0x33
} ND_PacketType;

typedef struct {
	ND_PacketType  packetType;
	IPAddress      addressIP;	                // local IP address
	uint8_t        addressMAC[6];	             // local MAC address
	uint8_t        payload[8];                 // user-defined payload component
} ND_Packet;

class NetDiscovery {
public:
	// user functions
	bool begin (const IPAddress multicastIP, const int mcastPort);
	bool listen (const ND_PacketType packetType, ND_Packet *packet);
	bool announce (const ND_Packet *packet);
	bool ack (const ND_Packet *packet);


private:
	// for multicast
	WiFiUDP   _mcast;	                 // multicast UDP stream
	IPAddress _mcastIP;                // multicast group address 239.X.Y.Z
	int       _mcastPort;              // multicast port number

	// user context
	IPAddress _localIP;
	uint8_t   _localMAC[6];

	inline bool _inRange (const int octet);

};
#endif