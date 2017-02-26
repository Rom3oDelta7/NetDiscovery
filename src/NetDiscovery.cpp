/*
Network Discovery Library

NetDiscovery is an Arduino library for the ESP8266 that implements a simple UDP multicast discovery protocol on a WiFi network.
This enables devices on a network to find each other and exchange inforamtion to facilitate their communication.
The simplest example of this is a set of devices that need to commuicate through IP but don't
initially know the IP addresses of the other devices.
Eight bytes of application-specific information may be added to the packet payload field for exchange amongst the devices.
This library can be used to implement a master/slave or peer-to-peer topology for discovery.

Copyright 2017 Rob Redford
This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/4.0/.

Multicast intro: https://community.particle.io/t/multicast-udp-tutorial/19900
Reference: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/src

*/

#include "NetDiscovery.h"

/* 
 validate mcast IP address octet
*/
inline bool NetDiscovery::_inRange (const int octet) {
	return (octet >= 1) && (octet <= 255);
}

bool NetDiscovery::begin (const IPAddress multicastIP, const int mcastPort) {
	if ( (multicastIP[0] == 239) && _inRange(multicastIP[1]) && _inRange(multicastIP[2]) && _inRange(multicastIP[3]) ) {
		if ( (mcastPort >= ND_MCAST_PORT_LOW) && (mcastPort <= ND_MCAST_PORT_HIGH) ) {
			// init the class
			_mcastIP = multicastIP;
			_mcastPort = mcastPort;
			_localIP = WiFi.localIP();
			WiFi.macAddress(&_localMAC[0]);
			// join mcast group
			if (!_mcast.beginMulticast(_localIP, _mcastIP, _mcastPort)) {
				ND_DEBUG_MSG(1, F("Cannot join multicast group"), _mcastIP);
				return false;
			} else {
				return true;
			}
		}
	}
	ND_DEBUG_MSG(1, F("Begin"), F("malformed arguments"));
	return false;
}



/*
 listen for a packet
 if received, the packet contents will be copied to the given address and the type will be returned, otherwise 0
*/
uint8_t NetDiscovery::listen (ND_Packet *packet) {
	uint8_t pktType = 0;
	int     rcvSize = _mcast.parsePacket();                // get next mcast UDP packet

	if ( rcvSize ) {
		// packet received
		
		ND_DEBUG_MSG(2, F("Listen: Received packet of size"), rcvSize);
		ND_DEBUG_MSG(2, F("Listen: Received from"), _mcast.remoteIP());
		ND_DEBUG_MSG(2, F("Listen: Destination IP"), _mcast.destinationIP());
		ND_DEBUG_MSG(2, F("Listen: Remote port"), _mcast.remotePort());
		
		// check that this is the packet we want but leave the stream undisrurbed if it isn't
		if ( (_mcast.destinationIP() == _mcastIP) && (_mcast.remotePort() == _mcastPort) ) {
			// multicast group match
			int bytesRead;
			ND_Packet tmpPacket;
			
			if ( (bytesRead = _mcast.read((uint8_t *)&tmpPacket, sizeof(ND_Packet))) == sizeof(ND_Packet) ) {
				pktType = tmpPacket.packetType;
				ND_DEBUG_MSG(3, F("Listen: packet type"), pktType);
				memcpy((void *)packet, (void *)&tmpPacket, sizeof(ND_Packet));
				_mcast.flush();                    // should not be necessary  - just being paranoid
			} else {
				ND_DEBUG_MSG(1, F("Listen: packet read error. Count"), bytesRead);
				_mcast.flush();
			}
		}
	}
	return pktType;
}

/*
 send a packet of the specified type, copying in provided user payload
 all fields except the user-defined section are set by this function
 returns true if the packet was successfully sent
*/
bool NetDiscovery::send (const uint8_t packetType, const ND_Packet *packet) {
	bool      returnValue = false;
	ND_Packet tmpPacket;

	// send a packet
	if ( _mcast.beginPacketMulticast(_mcastIP, _mcastPort, _localIP) ) {
		size_t count;

		// initialize packet header
		tmpPacket.packetType = packetType;
		tmpPacket.addressIP = _localIP;
		memcpy((void*)&tmpPacket.addressMAC[0], (void*)&_localMAC[0], sizeof(tmpPacket.addressMAC));
		// get user payload
		memcpy((void *)&tmpPacket.payload[0], (void *)packet->payload, sizeof(tmpPacket.payload));
		if ( (count = _mcast.write((uint8_t *)&tmpPacket, sizeof(ND_Packet))) == sizeof(ND_Packet) ) {
			if ( _mcast.endPacket() ) {
				ND_DEBUG_MSG(3, F("Send"), F("sent OK"));
				returnValue = true;
			} else {
				ND_DEBUG_MSG(1, F("Send"), F("Cannot send packet"));
			}
		} else {
			ND_DEBUG_MSG(1, F("Send: write failed"), count);
		}
	} else {
		ND_DEBUG_MSG(1, F("Send"), F("Cannot create packet"));
	}
	return returnValue;
}

/*
 send announcement packet, copying in provided user payload
 all fields except the user-defined section are set by this function
 returns true if the packet was successfully sent
*/
bool NetDiscovery::announce (const ND_Packet *packet) {
	return send(ND_ANNOUNCE, packet);
}


/*
 create and send the given ACK packet
 all fields except the user-defined section are set by this function
 returns true if the packet was successfully sent
*/
bool NetDiscovery::ack (const ND_Packet *packet) {
	return send(ND_ACK, packet);
}

/*
 finish with the UDP multicast session
*/
void NetDiscovery::stop ( void ) {
	_mcast.stop();
}


