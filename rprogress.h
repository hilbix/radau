#include <unistd.h>

static void
r_progress(R)
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
}

