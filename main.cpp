#include <iostream>
#include <string>
#include "mac.h"
using namespace std;

string interface;
Mac ap_mac;
Mac station_mac = Mac::broadcastMac();
bool auth = false;

void usage()
{
    cout << "syntax : deauth-attack <interface> <ap mac> [<station mac> [-auth]]]\n";
    cout << "sample : deauth-attack mon0 00:11:22:33:44:55 66:77:88:99:AA:BB" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3 && argc != 4 && argc != 5)
    {
        usage();
        return -1;
    }

    interface = argv[1];
    ap_mac = Mac(argv[2]);
    if (argc >= 4)
        station_mac = Mac(argv[3]);
    if (argc == 5)
        if (string(argv[4]) == "-auth")
            auth = true;
}