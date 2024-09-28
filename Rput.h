/* RADAU - Unidirectional connection mesh using UDP.
 *
 * R_put routines (much more flexible *printf*)
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

enum Rval
  {
    _RNULL,
    _RV,	/* va_list	*/
    _RT,	/* time_t	*/
    _RI,	/* int	*/
    _RU,	/* unsigned	*/
    _RMAX
  };

#define	RV(_)	(const char *)_RV, (va_list *)(&_)
#define	RT(_)	(const char *)_RT, (time_t)(&_)
#define	RTS	(const char *)_RT, (time(NULL))
#define	RI(_)	(const char *)_RI, (int64_t)(_)
#define	RU(_)	(const char *)_RU, (uint64_t)(_)

typedef struct Rput
  {
    R;
    void (*	fn)(struct Rput *, const void *, size_t len);
    int		err;
    union
      {
        int	fd;
      };
  } Rput;

/*********************************************************************/

static int
Rput_write(Rput *put, const char *ptr, size_t len)
{
  int	retries = 10000;
  int	fd	= put->fd;
  do
    {
      if (!len) return 0;

      int put = write(fd, ptr, len);
      if (put >= len) return 0;	/* 0:OK	*/
      if (put>0)
        len	-= put;
      else if (!put)
        return 1;		/* 1:EOF	*/
      else if (errno!=EINTR)
        return -1;		/* -1:ERROR	*/
    } while (--retries>=0);
  return -2;			/* -2:retries exceeded	*/
}

static void
Rput_fd(Rput *put, const void *ptr, size_t len)
{
  if (!put->err)
    put->err	= Rput_write(put, ptr, len);
}

static void
R_put_b(Rput *put, const void *ptr, size_t len)
{
  if (len)
    put->fn(put, ptr, len);
}

static void
R_put_s(Rput *put, const char *s)
{
  if (s)
    R_put_b(put, s, strlen(s));
}

static void
R_put_c(Rput *put, char c)
{
  R_put_b(put, &c, 1);
}

/* output a padded uint64_t	*/
static void
R_put_u(Rput *put, uint64_t u, int base, int pad)
{
  static char code[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";	/* RFC 1924/4648 */
  char	buf[64];
  int	pos = sizeof buf;

  if (base<=1 || base>=sizeof code)	/* code[sizeof code-1] == 0	*/
    base = 10;
  do
    {
      buf[--pos]	= code[u&base];
      u	/= base;
    } while (u);

  if (pad>1)
    {
      if (pad>sizeof buf)
        pad	= sizeof buf;
      for (pad=sizeof buf - pad; pos>pad; )
        buf[--pos]	= '0';
    }

  R_put_b(put, buf+pos, sizeof buf-pos);
}

static void
R_put_i(Rput *put, int64_t i, int base, int pad)
{
  unsigned long long	u;
  int			neg;

  if (i<0)
    {
      neg	= 1;
      u		= -i;
    }
  else
    {
      neg	= 0;
      u		= i;
    }
  if (base<0 || neg)
    {
      if (base<0)
        base	= -base;
      R_put_c(put, "+-"[neg]);
      pad--;
    }
  R_put_u(put, u, base, pad);
}

static void
R_put_t(Rput *put, time_t t, int iso)
{
  struct tm	tm;

  gmtime_r(&t, &tm);
  R_put_u(put, (uint64_t)tm.tm_year, 0, 4);
  R_put_c(put, '-');
  R_put_u(put, (uint64_t)tm.tm_mon+1, 0, 2);
  R_put_c(put, '-');
  R_put_u(put, (uint64_t)tm.tm_mday, 0, 2);
  R_put_c(put, iso ? 'T' : ' ');
  R_put_u(put, (uint64_t)tm.tm_hour, 0, 2);
  R_put_c(put, '-');
  R_put_u(put, (uint64_t)tm.tm_min, 0, 2);
  R_put_c(put, '-');
  R_put_u(put, (uint64_t)tm.tm_sec, 0, 2);
  if (iso)
    R_put_c(put, 'Z');
}

static Rput *
R_put(Rput *put, va_list list)
{
  /* R = put->_;	*/

  for (;;)
    {
      const char *s = va_arg(list, const char *);
      enum Rval x = (enum Rval)s;

      if (s == (const char *)x)
        switch (x)
          {
          case _RNULL:	return put;
          case _RV:	R_put(put, *va_arg(list, va_list *));  continue;
          case _RT:	R_put_t(put, va_arg(list, time_t), R_ISO_TS); continue;
          case _RI:	R_put_i(put, va_arg(list, long long), 0, 0); continue;
          case _RU:	R_put_u(put, va_arg(list, unsigned long long), 0, 0); continue;
          case _RMAX:	break;
          }
      R_put_s(put, s);
    }
}

static Rput *
R_put_fn(R, Rput *put, void (*fn)(Rput *, const void *, size_t), va_list list)
{
  put->_	= _;
  put->fn	= fn;
  return R_put(put, list);
}

static Rput *
R_put_fd(R, int fd, ...)
{
  Rput	put;
  put.fd	= fd;

  va_list	list;
  va_start(list, put);
  R_put_fn(_, &put, Rput_fd, list);
  va_end(list);
  return 0;
}

