#define _GNU_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#include "radau.h"
#include "radau_version.h"

#include "rio.h"
#include "raddr.h"

#if 0
void
oops(const char *s)
{
  perror(s);
  exit(1);
}

int
main(int argc, char **argv)
{
  int			fd;
  struct addrinfo	*ret, hint;
  const char		*host, *port, *text;
  size_t		len;

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0))<0)
    oops("no socket");

  host	= argc>1 ? argv[1] : "localhost";
  port	= argc>2 ? argv[2] : "1111";
  text	= argc>3 ? argv[3] : "hello world";
  len	= strlen(text);

  memset(&hint, 0, sizeof hint);
  hint.ai_family	= AF_UNSPEC;
  hint.ai_flags		= AI_IDN;
  if (getaddrinfo(host, port, &hint, &ret))
    oops("cannot resolve");

  for (;;)
    {
      if (sendto(fd, text, len, 0, ret->ai_addr, ret->ai_addrlen)!=len)
	oops("send error");
      write(1, ".", 1);
      usleep(100000);
    }
}
#endif

void
r_init(R)
{
  memset(r, 0, sizeof *r);
}

int
r_exit(R)
{
  /* XXX TODO XXX implement this	*/
  return 0;
}

void
r_setup(R)
{
}

void
r_run(R)
{
}

int
main(int argc, char **argv)
{
  R;
  struct radau	radau;
  int		i;

  r = &radau;
  r_init(r);
  for (i=0; ++i<=argc; r_addr_add(r, argv[0]));
  r_setup(r, r_timer);
  r_run(r);
  return r_exit(r);
}

