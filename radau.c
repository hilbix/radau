#define _GNU_SOURCE

#include "tino/dirty.h"
#include "tino/file.h"
#include "tino/alloc.h"
#include "tino/fatal.h"
#include "tino/getopt.h"

#include "radau.h"
#include "radau_version.h"

#include "rio.h"
#include "raddr.h"
#include "rtimer.h"
#include "rprogress.h"

#if 0
int
main(int argc, char **argv)
{
  int			fd;
  struct addrinfo	*ret, hint;
  const char		*host, *port, *text;
  size_t		len;

  memset(&hint, 0, sizeof hint);
  hint.ai_family	= AF_UNSPEC;
  hint.ai_flags		= AI_IDN;
  if (getaddrinfo(host, port, &hint, &ret))
    oops("cannot resolve");

  for (;;)
    {
      if (sendto(fd, text, len, 0, ret->ai_addr, ret->ai_addrlen)!=len)
        oops("send error");
    }
}
#endif

void
r_main_init(R)
{
  memset(r, 0, sizeof *r);
  r_addr_init(r);
  r_recv_init(r);
}

int
r_main_exit(R)
{
  /* XXX TODO XXX implement this	*/
  return 0;
}

void
r_setup(R)
{
  FATAL((r->sock = socket(AF_INET, SOCK_DGRAM, 0))<0);

  r_timer_setup(r);
  r_recv_setup(r);

  printf("%d addresses\n", r_ring_len(r->ring));
}

void
r_main(R)
{
  while (r->sock>=0)
    {
      r_recv();
      r_progress(r);
    }
}

int
main(int argc, char **argv)
{
  R;
  struct radau	radau;
  int		argn;

  argn	= tino_getopt(argc, argv, 0, -1,
                      TINO_GETOPT_VERSION(RADAU_VERSION)
                      " [destination]..",

                      TINO_GETOPT_USAGE
                      "h	this help"
                      ,

                      NULL
                      );
  if (argn<=0)
    return 1;

  r = &radau;
  r_main_init(r);
  for (; argn<argc; argn++)
    r_addr_add(r, argv[argn]);
  r_main_setup(r);
  r_main(r);
  return r_main_exit(r);
}

