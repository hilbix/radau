/* Address management	*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "rref.h"

static void
r_addr_free(R, void *ptr)
{
  freeaddrinfo(ptr);
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
  while (; ret; ret=ret->ai_next)
    r_ring_add(r, r->ring, ret, r_addr_cmp, refs);
  return 0;
}

