/*
 Network Discovery Library - a simple discovery protocol using UDP multicast

 Copyright 2017 Rob Redford
 This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 (CC BY-SA 4.0) International License.
 To view a copy of this license, visit https://creativecommons.org/licenses/by-sa/4.0

*/

#ifndef NETDISCOVERY_H
#define NETDISCOVERY_H

#if defined(ARDUINO) && (ARDUINO >= 10605)
#include "Arduino.h"
#else
#error This library requires Arduino IDE 1.6.5 or above
#endif

#ifndef ESP8266
#error This library is for ESP8266 only
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
#define ND_ANNOUNCE          0x11
#define ND_ACK               0x22
#define ND_INFO              0x33              	

typedef struct {
	uint8_t        packetType;
	IPAddress      addressIP;	                         // local IP address
	uint8_t        addressMAC[WL_MAC_ADDR_LENGTH];	    // local MAC address
	uint8_t        payload[9];                          // user-defined payload component (pad to 4 byte boundry)
} ND_Packet;

class NetDiscovery {
public:
	// user functions
	bool    begin (const IPAddress multicastIP, const int mcastPort, const IPAddress localIP);
	uint8_t listen (ND_Packet *packet);
	bool    send (const uint8_t packetType, const ND_Packet *packet);
	bool    announce (const ND_Packet *packet);
	bool    ack (const ND_Packet *packet);
	void    stop (void);


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