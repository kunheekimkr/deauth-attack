#include "beacon.h"
#include "radiotap.h"

typedef struct deauthPacket
{
    radiotapHdr radio;
    beaconFrame beacon;
    uint16_t reasonCode;

} __attribute__((__packed__)) deauthPacket;