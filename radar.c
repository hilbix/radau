/* LIBS:	openssl ntru
 * DEBIAN:	libntru-0.5-dev libmsgpack-dev
 */

#define	GNU_SOURCE

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <ifaddrs.h>

#include "mem-tool.h"
#include "str-tool.h"
#include "pki-tool.h"

#define	RADAR_CONFIG	".radar.conf"

#define	R	struct radar *_
typedef struct radar_dest *Rdest;
struct radar
  {
    const char		*config;
    int			dstcnt;
    Rdest		*dst;
    PKI			pki;
    int			code;
    unsigned		allocated:1;
    unsigned		configproblems:1;
  };
struct radar_dest
  {
    R;
    struct addrinfo	*a;
  };

static void
Rerr(R, int e, const char *p, const char *s, va_list list)
{
  fprintf(stderr, "%s: ", p);
  vfprintf(stderr, s, list);
  if (e)
    fprintf(stderr, ": %s", strerror(e));
  fprintf(stderr, "\n");
}

static void
Roops(R, const char *s, ...)
{
  int		e = errno;
  va_list	list;

  va_start(list, s);
  Rerr(_, e, "OOPS", s, list);
  va_end(list);
  exit(23);
  abort();
  for (;;);
}

static void
Rwarn(R, const char *s, ...)
{
  int		e = errno;
  va_list	list;

  va_start(list, s);
  Rerr(_, e, "WARN", s, list);
  va_end(list);
}

static void
Rlog(R, const char *s, ...)
{
  int		e = errno;
  va_list	list;

  va_start(list, s);
  Rerr(_, e, "LOG", s, list);
  va_end(list);
}

static void
R_get_local(R)
{
  struct ifaddrs *list;

  if (getifaddrs(&list))
    Roops(_, "cannot get list of local interfaces");
}

static struct radar *
R_init(R)
{
  int	allocated = !_;

  if (allocated)
    _	= Malloc(sizeof *_);
  memset(_, 0, sizeof *_);
  _->allocated	= allocated;
  _->config	= Mdups(RADAR_CONFIG);
  R_get_local(_);
  _->pki	= PKI_init(Malloc(sizeof *_->pki), _);
  return _;
}

static int
R_exit(R)
{
  int	code;

  code	= _->code;
  PKI_free(_->pki);
  MFREE(_->pki);
  MFREE(_->config);
  return code;
}

static Rdest
Raddr(R, const char *s, int bind)
{
  char *h	= Mdups(s);
  char *p	= strrchr(h, ':');
  struct addrinfo	*a, o;
  Rdest		d;

  if (p)
    *p++ = 0;
  if (!p || !*p)
    p	= "51930";	/* 0xcada	*/

  memset(&o, 0, sizeof o);
  o.ai_family = AF_UNSPEC;
  o.ai_socktype = SOCK_DGRAM;
  o.ai_flags = bind ? AI_PASSIVE : 0;
  o.ai_protocol = 0;
  o.ai_canonname = NULL;
  o.ai_addr = NULL;
  o.ai_next = NULL;

  if (getaddrinfo(h, p, &o, &a))
    {
      Rwarn(_, "cannot get address %s (%s %s)", s, h, p);
      MFREE(h);
      return 0;
    }
  d	= Malloc(sizeof *d);
  d->a	= a;
  MFREE(h);
  return d;
}

static void
Raddr_add(R, const char *s, int bind)
{
  int	n;

  Rdest	x = Raddr(_, s, bind);
  if (!x) return;

  n	= ++_->dstcnt;
  Mrealloc(_->dst, n);
  _->dst[n-1]	= x;
}

/* config:
 *
 * IP:port	CommonSecret
 */
static void
R_config(R, const char * const *argv)
{
  int	i;
  FILE	*fd;
  int	fail;

  fail	= 1;
  if ((fd = fopen(_->config, "r")) != 0)
    {
      char	buf[BUFSIZ];
      int	cnt;

      for (;;)
        {
          if (!fgets(buf, sizeof buf, fd))
            Roops(_, "%s: unexpected end of config", _->config);
          if (!strncmp(buf, "#END#", 5))
            break;
          STR_rtrim(buf);
          if (!buf[0] || buf[0]=='#')
            continue;
#if 0
          pos	= spc(buf);
#endif
          Raddr_add(_, buf, 0);
        }
      sscanf(buf+5, "%d", &cnt);
      if (_->dstcnt != cnt)
        Roops(_, "%s: wrong number of entries in config: expected %d found %d", _->config, cnt, _->dstcnt);
      if (fgets(buf, sizeof buf, fd))
        {
          Rwarn(_, "%s: config does not end in #END#, automatic save disabled", _->config);
          _->configproblems = 1;
        }
      fail	= ferror(fd);
      fail	|= fclose(fd);
    }
  Rlog(_, "%s: config load %s: %d entries", _->config, fail ? "failed" : "succeeded", _->dstcnt);
  for (i=0; argv[++i]; )
    Raddr_add(_, argv[i], 0);
}

static char *
Rnameinfo(R, struct addrinfo *a)
{
  char	*h, *p, *r;

  h	= Malloc(BUFSIZ);
  p	= Malloc(BUFSIZ);
  r	= getnameinfo(a->ai_addr, a->ai_addrlen, h, BUFSIZ, p, BUFSIZ, NI_DGRAM|NI_NUMERICHOST|NI_NUMERICSERV)
        ? NULL
        : Mcatsn(h, ":", p, NULL);
  MFREE(h);
  MFREE(p);
  return r;
}

static void
R_config_save(R, int force)
{
  FILE	*fd;
  char	*name;
  int	fail;

  if (_->configproblems)
    {
      Rwarn(_, "%s: not saving config", _->config);
      return;
    }
  name	= Mcats(_->config, ".tmp");
  fail	= 1;
  errno	= 0;
  if ((fd = fopen(name, "w")) != 0)
    {
      int		i;
      struct addrinfo	*a;
      const char	*n;
      int		cnt;

      cnt	= 0;
      for (i=0; i<_->dstcnt && !ferror(fd); i++)
        for (a=_->dst[i]->a; a && !ferror(fd); a=a->ai_next)
          {
            n = Rnameinfo(_, a);
            if (n)
              {
                fprintf(fd, "%s\n", n);
                cnt++;
              }
            MFREE(n);
          }
      fprintf(fd, "#END#%d\n", cnt);
      fail	= ferror(fd);
      fail	|= fclose(fd);
    }
  if (!fail)
    fail	= rename(name, _->config);
  MFREE(name);
  Rlog(_, "%s: config save %s: %d entries", _->config, fail ? "failed" : "succeeded", _->dstcnt);
  MFREE(name);
}

int
main(int argc, const char * const *argv)
{
  struct radar r;
  R = &r;

  R_init(_);
  R_config(_, argv);
  R_config_save(_, 0);

  return R_exit(_);
}

