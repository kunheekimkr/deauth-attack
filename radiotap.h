#include <netinet/in.h>
// Source: https://www.radiotap.org/
typedef struct ieee80211_radiotap_header
{
    u_int8_t it_version; /* set to 0 */
    u_int8_t it_pad;
    u_int16_t it_len;     /* entire length */
    u_int32_t it_present; /* fields present */
    u_int8_t data_rate;
    u_int8_t zero;
    u_int16_t tx;
} __attribute__((__packed__)) radiotapHdr;