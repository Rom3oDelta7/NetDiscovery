/*
 Simple multicast test - receiver, which is listening for other device to announce their presence
 This code would be on the server (hub), for example

 Reference: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/src
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "NetDiscovery.h"
#include "common.h"
#include <array>

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
	DEBUG_MSG(1, F("RECEIVER Connected. Local IP"), WiFi.localIP());

	if ( !discovery.begin(mcastIP, MCAST_PORT) ) {       // join mcast group
		DEBUG_MSG(1, F("Cannot initialize discovery mcast group"), mcastIP);
		while ( true ) delay(1000);
	}
}

/*
 simple test where the packet contains just the IP address and an ID
 loops forever - your application logic would need to determine how the loop is terminated
*/
void loop()
{
	ND_Packet localPacket, remotePacket;
	std::array<bool, 256> senderACK;


	DEBUG_MSG(1, F("RECEIVER listening"), "");
	senderACK.fill(false);
	while ( true ) {
		// listen for announcement packets & ACK it
		Serial.print(".");
		if ( discovery.listen(&remotePacket) == ND_ANNOUNCE ) {
			uint8_t senderID = remotePacket.payload[0];           // ID is the last octet in the IP address for unique identification

			if ( !senderACK[senderID] ) {
				// we have not yet acknowledged this sender
				localPacket.payload[0] = senderID;                 // return sender's ID
				if ( discovery.ack(&localPacket) ) {
					Serial.print(F("Discovered device at "));
					Serial.println((IPAddress)remotePacket.addressIP);
					Serial.print(F("Remote MAC: "));
					for ( int i = 0; i < WL_MAC_ADDR_LENGTH; i++ ) {
						Serial.print(remotePacket.addressMAC[i], HEX);
						if ( i < WL_MAC_ADDR_LENGTH - 1 ) {
							Serial.print(".");
						}
					}
					senderACK[senderID] = true;
					Serial.println();
				}
			}
			
		}
		yield();
		delay(1000);
	}
}
