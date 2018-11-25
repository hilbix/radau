/* basic IO (console)
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if	RADAU_PHASE==1

#if 0
void (*out)(R, const char *what, const char *msg, va_list list, int e);
#endif
void (*warn)(R, const char *msg, ...);
void (*info)(R, const char *msg, ...);

#elif	RADAU_PHASE==2

static void
r_con_vout(R, const char *what, const char *msg, va_list list, int e)
{
  fprintf(stderr, "%s: ", what);
  vfprintf(stderr, msg, list);
  if (errno)
    fprintf(stderr, ": %s", strerror(e));
  fprintf(stderr, "\n");
}

static void
r_con_warn(R, const char *msg, ...)
{
  va_list	list;

  va_start(list, msg);
  r_con_vout(r, "WARN", msg, list, 0);
  va_end(list);
}

static void
r_con_info(R, const char *msg, ...)
{
  va_list	list;

  va_start(list, msg);
  r_con_vout(r, "INFO", msg, list, 0);
  va_end(list);
}

static void
r_con_setup(R, RMODULE)
{
  r->warn	= r_con_warn;
  r->info	= r_con_info;
}

#elif	RADAU_PHASE==3

R_MODULE(r_con);

#endif

