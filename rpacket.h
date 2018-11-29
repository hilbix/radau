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

char			packet[R_PACKET_IP_MAX_SZ];
struct tino_md5_context	whirl;

#elif	RADAU_PHASE==RADAU_PHASE_CODE

static void
r_packet_whirl(R, const void *ptr, size_t len)
{
  struct timespec	ts;

  clock_gettime(CLOCK_REALTIME, &ts);
  tino_md5_update(&r->whirl, ptr, len);
  tino_md5_update(&r->whirl, &ts, sizeof ts);
}

static int
r_packet_whirl_s(R, const char *s)
{
  r_packet_whirl(r, s, strlen(s));
  return 0;
}

static void
r_packet_whirl_init(R)
{
  struct
    {
      struct timeval	tv;
      struct timezone	tz;
      struct utsname	uts;
      int		i[3];
      pid_t		pid, ppid;
    } w;

  w.pid		= getpid();
  w.ppid	= getppid();
  w.i[0]	= gettimeofday(&w.tv, &w.tz);
  w.i[1]	= errno;
  w.i[2]	= uname(&w.uts);

  r_packet_whirl(r, &w, sizeof w);
}

static void
r_packet_read(R, struct rconfig_read *c)
{
  c->read(c, "random", r_packet_whirl_s);
}

static void
r_packet_write(R, struct rconfig_write *c)
{
  struct tino_md5_context	tmp;
  char				out[33];

  tmp	= r->whirl;
  tino_md5_hex(&tmp, out);
  c->write(c, "random", out);
  r_packet_whirl_s(r, out);
}

static void
r_packet_setup(R, RMODULE)
{
  m->read       = r_packet_read;
  m->write      = r_packet_write;

  tino_md5_init(&r->whirl);
  r_packet_whirl_init(r);
}

#endif

