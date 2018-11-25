/* Outgoing packets
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

RADAU_MODULE(r_out)

#if	RADAU_PHASE==RADAU_PHASE_CONFIG

int	sock;

#elif	RADAU_PHASE==RADAU_PHASE_CODE

static void
r_out_open(R)
{
  FATAL((r->sock = socket(AF_INET, SOCK_DGRAM, 0))<0);
}

static void
r_out_close(R)
{
  tino_file_closeE(r->sock);
  r->sock	= -1;
}

static void
r_out(R)
{
  const struct addrinfo	*a;

  a	= r_addr_next(r);
  if (!a)
    {
      r->err(r, "no addresses given");
      r_out_close(r);
      return;
    }
  printf("%s\n", r_addr_name(r, a, 0));
  fflush(stdout);
#if 0
      if (sendto(fd, text, len, 0, ret->ai_addr, ret->ai_addrlen)!=len)
#endif
}

static void
r_out_init(R, RMODULE)
{
  r_out_open(r);
}

static void
r_out_exit(R, RMODULE)
{
  r_out_close(r);
}

static void
r_out_setup(R, RMODULE)
{
  m->init	= r_out_init;
  m->exit	= r_out_exit;
}

#elif	RADAU_PHASE==RADAU_PHASE_GETOPT

#endif

