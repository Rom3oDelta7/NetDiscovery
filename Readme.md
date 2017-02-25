# Network Discovery Library for ESP8266 WiFi
NetDiscovery is an Arduino library for the ESP8266 that implements a simple UDP multicast discovery protocol on a WiFi network.
This enables devices on a network to find each other and exchange inforamtion to facilitate their communication.
The simplest example of this is a set of devices that need to commuicate through IP but don't
initially know the IP addresses of the other devices.
You can exchange any type of information: you call the functions with a pointer to the data you want to be copied into either the announcement or ACK packets.
This library can be implemented using a master/slave or peer-to-peer topology.

# Programming Interface
## Initialization
First, the NetDiscovery instance must be declared. For example,
```C++
NetDiscovery discovery;
```
## Functions
```C++
bool NetDiscovery::begin (const IPAddress localIP, const IPAddress multicastIP, const int mcastPort);
```
Initialize the multicast group.
Parameters are as follows:

|Parameter|Purpose|
|---|---|
|_localIP_|the IP address of the device calling the function|
|_multicastIP_|the UDP multicast group address. This must be of the form 239.X.Y.Z where X, Y, and Z are within the range 1:255.|
|_mcastPort_|the UDP multicast port number. This must be a number within the range ND_MCAST_PORT_LOW:ND_MCAST_PORT_HIGH.| 

Returns __true__ if the multicast group was successfully established, else __false__.

```C++
bool NetDiscovery::listen (const IPAddress localIP, void *packet, const size_t packetSize, const void *ackPacket, const size_t ackPacketSize);
```

Listen for incoming announcement packets, and send an acqknowledgement (ACK) packet when one has been received.
Parameters are as follows:

|Parameter|Purpose|
|---|---|
|_localIP_|the IP address of the device calling the function|
|_packet_|incoming annoncement packets will be copied to this address (topology information __received__)|
|_packetSize_|number of bytes to be copied from the incoming multicast packet to the address pointed to by _packet_|
|_ackPacket_|pointer to the data to be returned in an ACK packet to the sender after an accouncement packet has been received (e.g. topology information __sent__)|
|_ackPacketSize_|number of bytes in the ACK packet|

Returns __true__ if an announcement packet was received and an ACK packet was sent, else __false__.

```C++
bool NetDiscovery::announce (const IPAddress localIP, const void *packet, const size_t packetSize, void *ackPacket, const size_t ackPacketSize);
```

Send an announcement packet and get the corresponding ACK packet.
Parameters are as follows:

|Parameter|Purpose|
|---|---|
|_localIP_|the IP address of the device calling the function|
|_packet_|pointer to the data to be copied into the announcement packet (topology information __sent__)|
|_packetSize_|number of bytes to be copied|
|_ackPacket_|pointer to the incoming announcement acknowledgement packet (e.g. topology information __received__)|
|_ackPacketSize_|number of bytes in the ACK packet|

Returns __true__ if an announcement packet was sent and an acknowledgement packet was successfully read, else __false__.

# Examples
The _examples_ folder contains sample code for a receiver (master) and a sender (slave/client) that exchange their respective IP addresses.
Each must be downloaded to its own ESP8266 with both running on the same local WiFi network.
The receiver (master) will listen for announcement packets and ACK them;
the sender will announce itself and then get the ACK.
For simplicity, the loop terminates once each device has discovered the other.

# Installation

Go to "Releases" and download the latest release .zip archive.
Install by using the "Sketch > Include Library > Add .ZIP Library..." menu commands in the Arduino IDE.

Of course, you may also clone this repository and manually install the files if you prefer.

# Copyright Notice

Copyright 2017 Rob Redford
This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
To view a copy of this license, visit [BY-NC-SA].