/*
Network Discovery Library

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

bool NetDiscovery::begin (const IPAddress localIP, const IPAddress multicastIP, const int mcastPort) {
	if ( (multicastIP[0] == 239) && _inRange(multicastIP[1]) && _inRange(multicastIP[2]) && _inRange(multicastIP[3]) ) {
		if ( (mcastPort >= ND_MCAST_PORT_LOW) && (mcastPort <= ND_MCAST_PORT_HIGH) ) {
			_mcastIP = multicastIP;
			_mcastPort = mcastPort;
			if (!_mcast.beginMulticast(localIP, _mcastIP, _mcastPort)) {       // join mcast group
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
 returns true if an announcement packet was received and an ACK was successfuly sent
 the received announcement packet is saved using the supplied pointer (*packet) and the provided ACK packet (ackPacket) is sent to the receiver
*/
bool NetDiscovery::listen (const IPAddress localIP, void *packet, const size_t packetSize, const void *ackPacket, const size_t ackPacketSize) {
	bool returnValue = false;
	int rcvSize = _mcast.parsePacket();                // get next mcast UDP packet

	if ( rcvSize ) {
		// packet received
		ND_DEBUG_MSG(2, F("Listen: Received packet of size"), rcvSize);
		ND_DEBUG_MSG(2, F("Listen: Received from"), _mcast.remoteIP());
		ND_DEBUG_MSG(2, F("Listen: Destination IP"), _mcast.destinationIP());
		ND_DEBUG_MSG(2, F("Listen: Remote port"), _mcast.remotePort());

		if ( (_mcast.destinationIP() == _mcastIP) && (_mcast.remotePort() == _mcastPort) ) {
			// multicast group match, otherwise not for us
			int bytesRead;

			if ( (bytesRead = _mcast.read((uint8_t *)packet, packetSize)) == packetSize ) {
				// got it - send ACK packet in response
				ND_DEBUG_MSG(3, F("Listen"), F("annoucement packet received"));
				if ( _mcast.beginPacketMulticast(_mcastIP, _mcastPort, localIP) ) {
					size_t count;
					if ( (count = _mcast.write((uint8_t *)ackPacket, ackPacketSize)) == ackPacketSize ) {
						if ( _mcast.endPacket() ) {
							ND_DEBUG_MSG(3, F("Listen"), F("ACK'd announcement"));
							returnValue = true;
						} else {
							ND_DEBUG_MSG(1, F("Listen"), F("Cannot send ACK packet"));
						}
					} else {
						ND_DEBUG_MSG(1, F("Listen: Failed writing ACK packet"), count);
					}
				} else {
					ND_DEBUG_MSG(1, F("Listen"), F("Cannot create ACK packet"));
				}
			} else {
				ND_DEBUG_MSG(1, F("Listen: announcement pkt read error. Count"), bytesRead);
				_mcast.flush();
			}
		}
	}
	return returnValue;
}

/*
send provided discovery announcement packet (packet) and return ACK packet response (*ackPacket)
returns true if the packet was successfully sent and an ACK was received
*/
bool NetDiscovery::announce (const IPAddress localIP, const void *packet, const size_t packetSize, void *ackPacket, const size_t ackPacketSize) {
	bool returnValue = false;

	// send announcement packet
	if ( _mcast.beginPacketMulticast(_mcastIP, _mcastPort, localIP) ) {
		size_t count;
		if ( (count = _mcast.write((uint8_t *)packet, packetSize)) == packetSize ) {
			if ( _mcast.endPacket() ) {               // send packet
				// send successful - now get ACK from receiver
				ND_DEBUG_MSG(3, F("Announce"), F("announcement packet sent"));
				int rcvSize = _mcast.parsePacket();
				if ( rcvSize ) {
					ND_DEBUG_MSG(2, F("Announce: Received packet of size"), rcvSize);
					ND_DEBUG_MSG(2, F("Announce: Received from"), _mcast.remoteIP());
					ND_DEBUG_MSG(2, F("Announce: Destination IP"), _mcast.destinationIP());
					ND_DEBUG_MSG(2, F("Announce: Remote port"), _mcast.remotePort());

					if ( (_mcast.destinationIP() == _mcastIP) && (_mcast.remotePort() == _mcastPort) ) {
						int bytesRead;
						if ( (bytesRead = _mcast.read((uint8_t *)ackPacket, ackPacketSize)) == ackPacketSize ) {
							ND_DEBUG_MSG(3, F("Announce"), F("ACK received"));
							returnValue = true;
						} else {
							ND_DEBUG_MSG(1, F("Announce: Failed reading ACK. Count"), bytesRead);
							_mcast.flush();
						}
					}
				}
			} else {
				ND_DEBUG_MSG(1, F("Announce"), F("Cannot send announcement packet"));
			}
		} else {
			ND_DEBUG_MSG(1, F("Announce: Failed writing data to packet"), count);
		}
	} else {
		ND_DEBUG_MSG(1, F("Announce"), F("Cannot create announcement packet"));
	}
	return returnValue;
}
