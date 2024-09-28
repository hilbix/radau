/* RADAU - Unidirectional connection mesh using UDP.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#define	GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>

struct radau;
#define	R	struct radau *_

/*********************************************************************/

#ifndef	R_ISO_TS
#define	R_ISO_TS	1
#endif

#ifndef	R_FAIL_AND_NEVER_RETURN
static void R_FAIL_AND_NEVER_RETURN(R) { for (;; abort()) exit(23); }	/* fail and never return	*/
#endif

/*********************************************************************/

struct radau
  {
    int			code;
    struct Rlist	*scripts;
    pthread_t		main;
  };

typedef struct Rlist
  {
    R;
    struct Rlist	*next, *prev;
    size_t		len;
  } Rlist;
 
struct Rscript
  {
    struct Rlist	L;
  };

#define	ROOPS(...)		R_oops(_, __FILE__, ":", RI(__LINE__), " @", __func__, ": ", __VA_ARGS__, NULL);
#define	RFATAL(COND, ...)	do { if (COND) ROOPS(#COND, __VA_ARGS__); } while (0)
#define	RLOG(fd, ...)		R_log(_, fd, __VA_ARGS__, NULL);
#define	RWARN(COND, ...)	R_warn(_, __VA_ARGS__, NULL);

#define	RISMAIN (_->main == pthread_self())

/*********************************************************************/

#include "Rput.h"

/*********************************************************************/

static void
R_log(R, int fd, ...)
{
  va_list	list;

  va_start(list, fd);
  R_put_fd(_, fd, RTS, RV(list), "\n", NULL);
  va_end(list);
}

static void	/* never returns	*/
R_oops(R, ...)
{
  va_list	list;

  va_start(list, _);
  RLOG(2, "OOPS: ", RV(list));
  va_end(list);
  R_FAIL_AND_NEVER_RETURN(_);	/* never returns	*/
}

static void *
Rrealloc(R, void *ptr, size_t len)
{
  void	*ret;

  ret	= ptr ? realloc(ptr, len) : malloc(len);
  if (!ret) ROOPS("out of memory allocating ", RU(len), " bytes");
  return ptr;
}

static void *
Ralloc(R, size_t len)
{
  return Rrealloc(_, NULL, len);
}

static void *
Ralloc0(R, size_t len)
{
  void *p	= Ralloc(_, len);
  memset(p, 0, len);
  return p;
}

#define	RLIST(TYPE,list)	TYPE list; R_list_init(_, &list.L, sizeof(TYPE))

static void
R_list_init(R, Rlist *list, size_t len)
{
  memset(list, 0, len);
  list->_	= _;
  list->len	= len;
}

typedef struct Rbuf
  {
    Rlist	L;
    char	*data;
    int		pos, fill, max;
  } Rbuf;

static Rbuf
R_vbuf(R, va_list list)
{
  Rbuf	*ret;
  RLIST(Rbuf, buf);




}

static Rbuf
R_buf(R, ...)
{
  Rbuf	*ret;
  RLIST(Rbuf, buf);
  va_start(list, _);
  ret	= R_vbuf(_, list);
  va_end(list);
  return ret;
}

static void
R_note(R, ...)
{
  va_list	list;

  if (RISMAIN)
    {
       va_start(list, _);
       buf	= R_vbuf(_, list);
       va_end(list);

    }
#if 0
      return Rnote(1, R_buf->data, R_buf->len);
#endif
}

static void
R_warn(R, ...)
{
  va_list	list;

  va_start(list, R);
  R_vwarn(R, list, NULL);
  va_end(list);
}

static void
Rclose(R, int fd)
{
  if (!fd) return;	/* we never close stdin so we can use 0 as undefined FD	*/
#if 0
  while (close(fd) && errno == EINTR)
#else
  /* Linux closes the FD even with EINTR, see man 2 close.
   * Hence we must not retry close() as we are multithreaded.
   * This may leak FDs on other platforms.
   */
  if (close(fd))
    RWARN("error closing fd ", RU(fd), ": ", strerror(errno));
#endif
}

/*********************************************************************/

static void *
R_list(R, size_t len)
{
  struct Rlist	*l;

  l		= Ralloc0(_, len);
  l->_		= _;
  return l;
}

/* Run a standard script with arguments:
 *
 * - Create 2 pipes
 * - Create a new socketpair
 * - Create a new thread
 */ 
static void
R_script(R, const char *name, const char * const *args)
{
  int	fds[3][2];

  memset(fds, 0, sizeof fds);
  if (pipe(fds[0]) || pipe(fds[1]) || socketpair(AF_UNIX, SOCK_STREAM, 0, fds[2])) goto fail;

fail:
  Rclose(fds[0][0]);
  Rclose(fds[0][1]);
  Rclose(fds[1][0]);
  Rclose(fds[1][1]);
  Rclose(fds[2][0]);
  Rclose(fds[2][1]);
  return 0;
}

static int
R_loop(R)
{
}

static void
R_init(R)
{
  memset(_, 0, sizeof *_);
  _->main	= pthread_self();
}

static int
R_main(R, const char * const * args)
{
  R_init(_);
  R_script(_, "config.radau", args);                                                                                                      
  while (R_loop(_));
  return _->code;       /* never reached        */
}

