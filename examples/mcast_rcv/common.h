/*

Common definitions

*/

#define MCAST_PORT     7245
#define MCAST_ADDRESS  239, 1, 4, 77

#define SSID           "<enter SSID here>"
#define PASSWORD       "<enter password here>"


// this data structure contains the information to be exchanged between devices
typedef struct {
	IPAddress address;
} Payload;
