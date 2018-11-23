/* Outgoing packets
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if	RADAU_PHASE==1

int	sock;

#elif	RADAU_PHASE==2

#include <unistd.h>

static void
r_out(R)
{
  const struct addrinfo	*a;
  char			host[4096], serv[100];

  a	= r_addr_next(r);

  if (getnameinfo(a->ai_addr, a->ai_addrlen, host, sizeof host, serv, sizeof serv, NI_NUMERICHOST | NI_NUMERICSERV))
    {
      perror("name resolution");
      return;
    }
  printf("%5s %s\n", serv, host);
  fflush(stdout);
#if 0
      if (sendto(fd, text, len, 0, ret->ai_addr, ret->ai_addrlen)!=len)
#endif
}

static void
r_out_init(R)
{
  FATAL((r->sock = socket(AF_INET, SOCK_DGRAM, 0))<0);
}

static void
r_out_exit(R)
{
  tino_file_closeI(r->sock);
  r->sock	= 0;
}

#elif	RADAU_PHASE==3

r->modadd(r, r_out_init, r_out_exit);

#endif

