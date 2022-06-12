/* Memory management
 */

#include <stdlib.h>

#define	MALLOC(P,N)	((P) = Mrealloc((P), (N)*sizeof *(P)))
#define	MFREE(P)	Mfree((void **)&(P))

#define	MEM	mem_239482394823894829wf9i239rk

static struct MEM
  {
    void	(*oom)(void *user);
    void	*user;
  } MEM;


static void
Minit(void *user, void (*oom)(void *))
{
  MEM.oom	= oom;
  MEM.user	= user;
}

static void *
Mrealloc(void *ptr, size_t len)
{
  void *r = ptr ? realloc(ptr, len) : malloc(len);
  if (!r && MEM.oom)
    MEM.oom(MEM.user);
  return r;
}

static void *
Malloc(size_t len)
{
  return Mrealloc(NULL, len);
}

static void *
Mfree(void **p)
{
  if (!p || !*p) return 0;
  free(*p);
  *p	= 0;
  return 0;
}

static char *
Mdups(const char *s)
{
  size_t	len;
  char		*r;

  len	= strlen(s)+1;
  r	= Malloc(len);
  memcpy(r, s, len);		/* strncpy() destroyed due to completely insane compiler warnings	*/
  r[len-1]	= 0;
  return r;
}

static char *
Mcats(const char *s, const char *a)
{
  size_t	ls, la;
  char		*r;

  ls	= strlen(s);
  la	= strlen(a)+1;
  r	= Malloc(ls+la);
  memcpy(r, s, ls);		/* strncpy() destroyed due to completely insane compiler warnings	*/
  memcpy(r+ls, a, la);		/* strncpy() destroyed due to completely insane compiler warnings	*/
  r[ls+la-1]	= 0;
  return r;
}

static char *
Mvcats(const char *s, va_list list)
{
  char		*x, *r;

  x	= Mdups(s);
  while ((s=va_arg(list, const char *))!=0)
    {
      r	= Mcats(x, s);
      MFREE(x);
      x	= r;
    }
  return x;
}

static char *
Mcatsn(const char *s, ...)
{
  va_list	list;
  char		*r;

  va_start(list, s);
  r	= Mvcats(s, list);
  va_end(list);
  return r;
}

static char *
Mvprintf(const char *s, va_list list)
{
  char	buf[1];
  int	n;

  if (!s)
    return 0;
  n	= vsnprintf(buf, sizeof buf, s, list);
  if (n<0)
    return 0;
  for (;;)
    {
      int	m;
      char	*r;

      r	= Malloc(++n);
      m	= vsnprintf(r, n, s, list);
      if (m<n)
        return r;
      MFREE(r);
      n	= m;		/* desparate	*/
    }
}

static char *
Mprintf(const char *s, ...)
{
  va_list	list;
  char		*r;

  va_start(list, s);
  r	= Mvprintf(s, list);
  va_end(list);
  return r;
}

#undef MEM

