/*
 Simple multicast test - receiver, which is listening for other device to announce their presence
 This code would be on the server (hub), for example

 Reference: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/src
 */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "NetDiscovery.h"
#include "common.h"

#define DEBUG 3

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
 simple test where the packet contains just the IP address
*/
void loop()
{
	ND_Packet localPacket, remotePacket;

	DEBUG_MSG(1, F("RECEIVER listening"), "");
	while ( true ) {
		// listen for announcement packets & ACK it
		Serial.print(".");
		if ( discovery.listen(ND_ANNOUNCE, &remotePacket) && (remotePacket.payload[0] == 0x7F) ) {
			localPacket.payload[0] = 0x3F;     // simple handshake example
			if ( discovery.ack(&localPacket) ) {
				Serial.print(F("Discovered device at "));
				Serial.println((IPAddress)remotePacket.addressIP);
				Serial.print("Remote MAC: ");
				for ( int i = 0; i < 6; i++ ) {
					Serial.print(remotePacket.addressMAC[i], HEX);
					if ( i < 5 ) {
						Serial.print(".");
					}
				}
				Serial.println();
				break;
			}
		}
		yield();
		delay(1000);
	}
	while ( true ) {
		delay(1000);
	}
}
