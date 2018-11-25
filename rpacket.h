/* packet rountines
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

RADAU_MODULE(packet)

#if	RADAU_PHASE==RADAU_PHASE_CONFIG

#define	R_PACKET_HD_IP_MIN	20
#define	R_PACKET_HD_IP_MAX	60
#define	R_PACKET_HD_UDP		8

#define	R_PACKET_IP4_MIN_SZ	576
#define	R_PACKET_IP6_MIN_SZ	1500
#define	R_PACKET_IP_MAX_SZ	65535

char	packet[R_PACKET_IP_MAX_SZ];

#elif	RADAU_PHASE==RADAU_PHASE_CODE

static void
r_packet_setup(R, RMODULE)
{
}

#endif

