/*
 Simple multicast test using a peer-to-peer topology
this code on each device you want to be discovered.

 
 To test hub-and-spoke, the other device should run the mcast_rcv sketch. 

 Reference: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/src
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "NetDiscovery.h"
#include "common.h"

//#define DEBUG 3                  // define as minimum desired debug level, or comment out to disable debug statements

#ifdef DEBUG
#define DEBUG_MSG(L, H, M)	       if ((L) <= DEBUG) {Serial.print("DEBUG> "); Serial.print(H); Serial.print(": "); Serial.println(M);}
#else
#define DEBUG_MSG(...)            ;
#endif

NetDiscovery       discovery;
IPAddress          mcastIP(MCAST_ADDRESS);               // multicast address

void setup()
{
	Serial.begin(115200);
	WiFi.begin(SSID, PASSWORD);
	while ( WiFi.status() != WL_CONNECTED ){
		delay(500);
		Serial.print(".");
	}
	Serial.println();
	DEBUG_MSG(1, F("PEER Connected. Local IP"), WiFi.localIP());

	if ( !discovery.begin(mcastIP, MCAST_PORT) ) {       // join mcast group
		DEBUG_MSG(1, F("Cannot initialize discovery mcast group"), mcastIP);
		while ( true ) delay(1000);               // ESP will get a WDT reset if you don't have something in the loop
	}
}

/*
 simple test with a simple user payload
 peer-to-peer topology, so we need to both announce our presence and listen/ack for others
 we use the last octet of the IP address to discriminate between multiple peers that may be in the discovery process
*/
void loop()
{
	ND_Packet localPacket, remotePacket;
	bool      announced = false;
	uint8_t   pktType;
	IPAddress localIP = WiFi.localIP();

	DEBUG_MSG(1, F("Peer starting"), localIP[3]);
	
	// this just loops forever - your application logic needs to determine when this loop should terminate
	while ( true ) {
		Serial.print(F("."));
		if ( !announced ) {
			localPacket.payload[0] = localIP[3];      // each peer has a different ID
			discovery.announce(&localPacket);
		}
		pktType = discovery.listen(&remotePacket);
		switch ( pktType ) {
		case ND_ACK:
			// we have been acknowledged by a peer
			DEBUG_MSG(1, F("ACK"), F("received"));
			if ( announced ) {
				DEBUG_MSG(1, F("ACK"), F("ignored"));
			} else if ( remotePacket.payload[0] == localIP[3] ) {            // this is our ACK
				Serial.print(F("Discovered device at "));
				Serial.println((IPAddress)remotePacket.addressIP);
				Serial.print(F("Remote MAC: "));
				for ( int i = 0; i < WL_MAC_ADDR_LENGTH; i++ ) {
					Serial.print(remotePacket.addressMAC[i], HEX);
					if ( i < WL_MAC_ADDR_LENGTH-1 ) {
						Serial.print(".");
					}
				}
				announced = true;
				Serial.println();
			}
			break;
			
		case ND_ANNOUNCE:
			DEBUG_MSG(1, F("ANNOUNCE"), F("received"));
			localPacket.payload[0] = remotePacket.payload[0];                // retain the peer ID of the sender
			if ( discovery.ack(&localPacket) ) {
				DEBUG_MSG(1, F("ACK"), F("sent"));
			}
			break;
			
		case 0:
		default:
			break;
		}	
		yield();
		delay(1000);
	}
}
