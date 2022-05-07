#define	GNU_SOURCE

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define	RADAR_CONFIG	".radar.conf"

#define	R	struct radar *_
typedef struct radar_dest *Rdest;
struct radar
  {
    const char		*config;
    int			dstcnt;
    Rdest		*dst;
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

static void *
R_alloc(R, void *ptr, size_t len)
{
  void *r = ptr ? realloc(ptr, len) : malloc(len);
  if (!r)
    Roops(_, "out of memory");
  return r;
}

static void *
Ralloc(R, size_t len)
{
  return R_alloc(_, NULL, len);
}

#define	Realloc(_,P,N)	do { P = R_alloc(_, (P), (N)*sizeof *(P)); } while (0)
#define	Rfree(_,P)	R_free(_,(void **)&(P))

static void
R_free(R, void **p)
{
  if (!p || !*p) return;
  free(*p);
  *p	= 0;
}

static char *
Rdup(R, const char *s)
{
  size_t	len;
  char		*r;

  len	= strlen(s)+1;
  r	= Ralloc(_, len);
  memcpy(r, s, len);		/* strncpy() destroyed due to completely insane compiler warnings	*/
  r[len-1]	= 0;
  return r;
}

static char *
Rcat(R, const char *s, const char *a)
{
  size_t	ls, la;
  char		*r;

  ls	= strlen(s);
  la	= strlen(a)+1;
  r	= Ralloc(_, ls+la);
  memcpy(r, s, ls);		/* strncpy() destroyed due to completely insane compiler warnings	*/
  memcpy(r+ls, a, la);		/* strncpy() destroyed due to completely insane compiler warnings	*/
  r[ls+la-1]	= 0;
  return r;
}

static char *
Rcats(R, const char *s, ...)
{
  va_list	list;
  char		*x, *r;

  x	= Rdup(_, "");
  for (va_start(list, s); s; s=va_arg(list, const char *))
    {
      r	= Rcat(_, x, s);
      Rfree(_, x);
      x	= r;
    }
  va_end(list);
  return x;
}

static struct radar *
R_init(R)
{
  int	allocated = !_;

  if (allocated)
    _	= Ralloc(_, sizeof *_);
  memset(_, 0, sizeof *_);
  _->allocated	= allocated;
  _->config	= Rdup(_, RADAR_CONFIG);
  return _;
}

static int
R_exit(R)
{
  int	code;

  code	= _->code;
  Rfree(_, _->config);
  return code;
}

static Rdest
Raddr(R, const char *s, int bind)
{
  char *h	= Rdup(_, s);
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
      Rwarn(_, "cannot get address %s", s);
      Rfree(_, h);
      return 0;
    }
  d	= Ralloc(_, sizeof *d);
  d->a	= a;
  Rfree(_, h);
  return d;
}

static void
Raddr_add(R, const char *s, int bind)
{
  int	n;

  Rdest	x = Raddr(_, s, bind);
  if (!x) return;

  n	= ++_->dstcnt;
  Realloc(_, _->dst, n);
  _->dst[n-1]	= x;
}

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

  h	= Ralloc(_, BUFSIZ);
  p	= Ralloc(_, BUFSIZ);
  r	= getnameinfo(a->ai_addr, a->ai_addrlen, h, BUFSIZ, p, BUFSIZ, NI_DGRAM|NI_NUMERICHOST|NI_NUMERICSERV)
        ? NULL
        : Rcats(_, h, ":", p, NULL);
  Rfree(_, h);
  Rfree(_, p);
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
  name	= Rcat(_, _->config, ".tmp");
  fail	= 1;
  errno	= 0;
  if ((fd = fopen(name, "w")) != 0)
    {
      int		i;
      struct addrinfo	*a;
      const char	*n;
      int		cnt;

      cnt	= 0;
      for (i=0; i<_->dstcnt; i++)
        for (a=_->dst[i]->a; a; a=a->ai_next)
          {
            n = Rnameinfo(_, a);
            if (n)
              {
                fprintf(fd, "%s\n", n);
		cnt++;
                if (ferror(fd))
                  break;
              }
            Rfree(_, n);
          }
      fprintf(fd, "#END#%d\n", cnt);
      fail	= ferror(fd);
      fail	|= fclose(fd);
    }
  if (!fail)
    fail	= rename(name, _->config);
  Rfree(_, name);
  Rlog(_, "%s: config save %s: %d entries", _->config, fail ? "failed" : "succeeded", _->dstcnt);
  Rfree(_, name);
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

