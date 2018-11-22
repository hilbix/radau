/* basic IO	*/

#include "radau.h"

#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

static void
vOUT(R, const char *what, const char *msg, va_list list, int e)
{
  fprintf(stderr, "%s: ", what);
  vfprintf(stderr, msg, list);
  if (errno)
    fprintf(stderr, ": %s", strerror(e));
  fprintf(stderr, "\n");
}

static void
WARN(R, const char *msg, ...)
{
  va_list	list;

  va_start(list, msg);
  vOUT(r, "WARN", msg, list, 0);
  va_end(list);
}

