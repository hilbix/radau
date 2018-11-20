/* Address management	*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "rref.h"
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

static int
r_addr_add(R, const char *s)
{
  struct addrinfo	*ret, hint;
  R_REF			refs;

  memset(&hint, 0, sizeof hint);
  hint.ai_family        = AF_UNSPEC;
  hint.ai_flags         = AI_IDN;
  if (getaddrinfo(s, STRINGIFY(R_PORT), &hint, &ret) || !ret)
    {
      WARN(r, "cannot resolve '%s'", s);
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

