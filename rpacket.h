/* packet rountines
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if	RADAU_PHASE==1

#define R_PACKET_HD_IP_MIN	20
#define R_PACKET_HD_IP_MAX	60
#define R_PACKET_HD_UDP	8

#define R_PACKET_IP4_MIN_SZ	576
#define R_PACKET_IP6_MIN_SZ	1500
#define R_PACKET_IP_MAX_SZ	65535

char	packet[R_PACKET_IP_MAX_SZ];

#elif	RADAU_PHASE==2

static void
r_packet_setup(R, RMODULE)
{
}

#elif	RADAU_PHASE==3

R_MODULE(r_packet);

#endif

