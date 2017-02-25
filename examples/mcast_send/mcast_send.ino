/*
 Simple multicast test - sender, which announces its presence on the network
 This code would be on the client, for example

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
	DEBUG_MSG(1, F("SENDER Connected. Local IP"), WiFi.localIP());

	if ( !discovery.begin(WiFi.localIP(), mcastIP, MCAST_PORT) ) {       // join mcast group
		DEBUG_MSG(1, F("Cannot initialize discovery mcast group"), mcastIP);
		while ( true ) delay(1000);               // ESP will get a WDT reset if you don't have something in the loop
	}
}

void loop()
{
	IPAddress local = WiFi.localIP(), remote;

	DEBUG_MSG(1, F("Sender starting"), "");
	while ( true ) {
		// announce our presence - this may need to happen multiple times until the receiver acknowledges us
		if ( discovery.announce(local, (void *)&local, sizeof(local), (void *)&remote, sizeof(remote)) ) {
			DEBUG_MSG(1, F("Discovered remote"), remote);
			break;
		} else {
			Serial.print(F("."));
			yield();
			delay(1000);
		}
	}
	Serial.println();
	while ( true ) delay(1000);
}
