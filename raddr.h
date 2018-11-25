/* Address management
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

RADAU_MODULE(r_addr)

#if	RADAU_PHASE==RADAU_PHASE_CONFIG

struct rring	*ring;

#elif	RADAU_PHASE==RADAU_PHASE_CODE

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
  refs	= r_ref(r, ret, 1, r_addr_free);
  for (; ret; ret=ret->ai_next)
    if (r_addr_valid(ret))
      r_ring_add(r, r->ring, ret, refs);
  r_ref_dec(r, refs);
  return 0;
}

static const struct addrinfo *
r_addr_next(R)
{
  struct addrinfo	*a;

  if (!r->ring || !r->ring->head)
    return 0;
  a		= r->ring->head->data;
  r->ring->head	= r->ring->head->next;
  return a;
}

static const char *
r_addr_name(R, const struct addrinfo *a, int names)
{
  char	host[4096], port[100];

  if (getnameinfo(a->ai_addr, a->ai_addrlen
                 , host, sizeof host
                 , port, sizeof port
                 , names ? 0 : (NI_NUMERICHOST | NI_NUMERICSERV)
     )           )
    {
      perror("name resolution");
      return 0;
    }
  return r->tmp(r, tino_str_printf("%s:%s", host, port));
}

static void
r_addr_exit(R, struct rmodule *m)
{
#if 0
  r_ring_free(r->ring);
#endif
  r->ring	= 0;
}

static void
r_addr_setup(R, struct rmodule *m)
{
  R_RING	a;

  m->exit	= r_addr_exit;

  a		= alloc0(sizeof *r);
  a->cmp	= r_addr_cmp;

  r->ring	= a;
}

#endif

