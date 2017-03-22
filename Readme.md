# Network Discovery Library for ESP8266 WiFi
NetDiscovery is an Arduino library for the ESP8266 that implements a simple UDP multicast discovery protocol on a WiFi network.
This enables devices on a network to find each other and exchange inforamtion to facilitate their communication.
The simplest example of this is a set of devices that need to commuicate through IP but don't
initially know the IP addresses of the other devices.
Nine bytes of application-specific information may be added to the packet payload field for exchange amongst the devices.
This library can be used to implement a master/slave or peer-to-peer topology for discovery. 
Examples of both topologies are provided.

# Programming Interface
## Initialization
First, the NetDiscovery instance must be declared. For example,
```C++
NetDiscovery discovery;
```
## Functions
```C++
bool NetDiscovery::begin (const IPAddress multicastIP, const int mcastPort, const IPAddress localIP);
```
Initialize the multicast group.
Parameters are as follows:

|Parameter|Purpose|
|---|---|
|_multicastIP_|the UDP multicast group address. This must be of the form 239.X.Y.Z where X, Y, and Z are within the range 1:255.|
|_mcastPort_|the UDP multicast port number. This must be a number within the range ND_MCAST_PORT_LOW:ND_MCAST_PORT_HIGH.|
|_localIP_|the IP address of the current device.|

Returns __true__ if the multicast group was successfully established, else __false__.

```C++
uint8_t NetDiscovery::listen (ND_Packet *packet);
```

Listen for an incoming packet. This function does not wait for a packet to arrive, and returns if one is not immediately available.
Parameters are as follows:

|Parameter|Purpose|
|---|---|
|_packet_|pointer to the packet received from the sender, which may be an announcement or ack packet

Returns the packet type (see _NetDiscovery.h_) if a packet was received, else 0.

```C++
bool NetDiscovery::announce (const ND_Packet *packet);
```

Send an announcement packet.
Parameters are as follows:

|Parameter|Purpose|
|---|---|
|_packet_|pointer to the packet containing the user payload to be transmitted in the annoucement packet|

Note that the _announce_ function will fill in the packet header fields;
the packet pointer supplied is only for the purposes of copying the user payload.
Returns __true__ if an announcement packet was sent, else __false__.

```C++
bool NetDiscovery::ack (const ND_Packet *packet);
```

Send an acknowledgement packet.
Parameters are as follows:

|Parameter|Purpose|
|---|---|
|_packet_|pointer to the packet containing the user payload to be transmitted in the annoucement packet|

Note that the _ack_ function will fill in the packet header fields;
the packet pointer supplied is only for the purposes of copying the user payload.
Returns __true__ if an ACK packet was sent, else __false__.

```C++
bool NetDiscovery::send (const uint8_t packetType, const ND_Packet *packet);
```

Lower level function to send a packet of an arbitrary type.
Parameters are as follows:

|Parameter|Purpose|
|---|---|
|_packetType_|packet type (see _NetDiscovery.h_) or you may define your own.|
|_packet_|pointer to the packet containing the user payload to be transmitted in the annoucement packet|

Note that the _send_ function will fill in the packet header fields;
the packet pointer supplied is only for the purposes of copying the user payload.
Returns __true__ if the packet was sent, else __false__.

```C++
void NetDiscovery::stop (void);
```

Terminates the UDP multicast stream and releases the corresponding resources.


# Examples
The _examples_ folder contains sample code for a receiver (master) and a sender (slave/client) that exchange their respective IP and MAC addresses.
Each must be downloaded to its own ESP8266 with both running on the same local WiFi network.
The receiver (master) will listen for announcement packets and ACK them;
the sender will announce itself and then get the ACK.

In addition, there is a peer-to-peer example, where you load and run the same sketch on at least 2 ESPs.

# Installation

Go to "Releases" and download the latest release .zip archive.
Install by using the "Sketch > Include Library > Add .ZIP Library..." menu commands in the Arduino IDE.

Of course, you may also clone this repository and manually install the files if you prefer.

# Copyright Notice

Copyright 2017 Rob Redford
This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
To view a copy of this license, visit [BY-SA].
[BY-SA]: https://creativecommons.org/licenses/by-sa/4.0/
