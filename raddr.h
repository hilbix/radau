/* Address management
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if	RADAU_PHASE==1

struct rring	*ring;

#elif	RADAU_PHASE==2

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "rring.h"

static void
r_addr_free(R, void *ptr)
{
  freeaddrinfo(ptr);
}

static int
r_addr_cmp(R, const void *A, const void *B)
{
  const struct addrinfo	*a=A, *b=B;

  if (a->ai_addr->sa_family != b->ai_addr->sa_family)
    return 1;
  switch (a->ai_addr->sa_family)
    {
    case AF_INET:
      if (((const struct sockaddr_in *)a->ai_addr)->sin_port != ((const struct sockaddr_in *)b->ai_addr)->sin_port)
        return 1;
      return memcmp(&((const struct sockaddr_in *)a->ai_addr)->sin_addr, &((const struct sockaddr_in *)b->ai_addr)->sin_addr, sizeof (struct in_addr));

    case AF_INET6:
      if (((const struct sockaddr_in6 *)a->ai_addr)->sin6_port != ((const struct sockaddr_in6 *)b->ai_addr)->sin6_port)
        return 1;
      return memcmp(&((const struct sockaddr_in6 *)a->ai_addr)->sin6_addr, &((const struct sockaddr_in6 *)b->ai_addr)->sin6_addr, sizeof (struct in6_addr));
    }
  return 1;
}

static int
r_addr_valid(struct addrinfo *a)
{
  switch (a->ai_addr->sa_family)
    {
    default:
      return 0;

    case AF_INET:
    case AF_INET6:
      break;
    }
  return 1;
}

static struct addrinfo *
r_addr_get(R, const char *s)
{
  struct addrinfo	*ret, hint;
  size_t		len;
  char			*host, *port;

  len   = strlen(s)+1;
  host  = (char *)TINO_F_alloca(len);
  memcpy(host, s, len);

  memset(&hint, 0, sizeof hint);
  hint.ai_family        = AF_UNSPEC;
  hint.ai_flags         = AI_IDN;

  /* Grok IPv6	*/
  if (*host=='[')
    {
      host++;
      hint.ai_family	= AF_INET6;
      port	= strchr(host, ']');
      if (!port)
        return 0;
      *port++	= 0;
    }
  else /* any	*/
    port	= strchr(host, ':');

  if (!port || !*port)
    port	= STRINGIFY(R_PORT);
  else if (*port!=':')
    return 0;
  else
    *port++	= 0;

  return getaddrinfo(host, port, &hint, &ret) ? 0 : ret;
}

static int
r_addr_add(R, const char *s)
{
  struct addrinfo	*ret;
  R_REF			refs;

  ret	= r_addr_get(r, s);
  if (!ret)
    {
      r->warn(r, "cannot resolve '%s'", s);
      return 1;
    }
  refs	= r_ref(r, ret, 0, r_addr_free);
  for (; ret; ret=ret->ai_next)
    if (r_addr_valid(ret))
      r_ring_add(r, r->ring, ret, refs);
  return 0;
}

static const struct addrinfo *
r_addr_next(R)
{
  struct addrinfo	*a;

  if (!r->ring->head)
    return 0;
  a	= r->ring->head->data;
  r->ring->head	= r->ring->head->next;
  return a;
}

static void
r_addr_init(R)
{
  R_RING	a;

  a		= alloc0(sizeof *r);
  a->cmp	= r_addr_cmp;

  r->ring	= a;
}

static void
r_addr_exit(R)
{
}

#elif	RADAU_PHASE==3

r->modadd(r, r_addr_init, r_addr_exit);

#endif

