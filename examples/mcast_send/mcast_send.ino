/*
 Simple multicast test - sender, which announces its presence on the network
 This code would be on the client, for example
 
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
	DEBUG_MSG(1, F("SENDER Connected. Local IP"), WiFi.localIP());

	if ( !discovery.begin(mcastIP, MCAST_PORT) ) {       // join mcast group
		DEBUG_MSG(1, F("Cannot initialize discovery mcast group"), mcastIP);
		while ( true ) delay(1000);               // ESP will get a WDT reset if you don't have something in the loop
	}
}

/*
 simple test with a simple user payload
*/
void loop()
{
	ND_Packet localPacket, remotePacket;
	IPAddress localIP = WiFi.localIP();
	bool      announced = false;

	localPacket.payload[0] = localIP[3];                 // unique peer ID in the case of multipe peers
	DEBUG_MSG(1, F("Sender starting"), localIP[3]);
	while ( !announced ) {
		// announce our presence - this may need to happen multiple times until the receiver acknowledges us
		Serial.print(F("."));
		if ( discovery.announce(&localPacket) ) {
			if ( discovery.listen(&remotePacket) == ND_ACK ) {
				DEBUG_MSG(1, "ACK", "received");
				if ( remotePacket.payload[0] == localIP[3] ) {    // is this ACK for us?
					Serial.print(F("Discovered device at "));
					Serial.println((IPAddress)remotePacket.addressIP);
					Serial.print(F("Remote MAC: "));
					for ( int i = 0; i < 6; i++ ) {
						Serial.print(remotePacket.addressMAC[i], HEX);
						if ( i < 5 ) {
							Serial.print(".");
						}
					}
					announced = true;
					discovery.stop();
					Serial.println();
				}
			}
		} 
		yield();
		delay(1000);
	}
	while ( true ) delay(1000);
}
