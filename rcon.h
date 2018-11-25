/* basic IO (console)
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

RADAU_MODULE(con)

#if	RADAU_PHASE==0

typedef void	R_CON(R, const char *msg, ...);

#elif	RADAU_PHASE==RADAU_PHASE_CONFIG

R_CON	*err, *warn, *info, *verbose, *verboser, *debug, *debuger;
int	flag_debug, flag_quiet, flag_verbose;

#elif	RADAU_PHASE==RADAU_PHASE_GETOPT

, TINO_GETOPT_FLAG "d	debug (does not include -v)"	, &r->flag_debug
, TINO_GETOPT_FLAG "q	suppress INFO/WARN"		, &r->flag_quiet
, TINO_GETOPT_FLAG "v	verbose"			, &r->flag_verbose

#elif	RADAU_PHASE==RADAU_PHASE_CODE

static void
r_con_vout(R, const char *what, const char *msg, va_list list, int e)
{
  if (what)
    fprintf(stderr, "%s: ", what);
  vfprintf(stderr, msg, list);
  if (e>=0)
    fprintf(stderr, ": %s", strerror(e));
  fprintf(stderr, "\n");
}

static void
r_con_err(R, const char *msg, ...)
{
  va_list	list;

  va_start(list, msg);
  r_con_vout(r, "ERROR", msg, list, errno);
  va_end(list);
}

static void
r_con_warn(R, const char *msg, ...)
{
  va_list	list;

  if (r->flag_quiet)
    return;
  va_start(list, msg);
  r_con_vout(r, "WARN", msg, list, errno);
  va_end(list);
}

static void
r_con_info(R, const char *msg, ...)
{
  va_list	list;

  if (r->flag_quiet)
    return;
  va_start(list, msg);
  r_con_vout(r, "INFO", msg, list, -1);
  va_end(list);
}

static void
r_con_verbose(R, const char *msg, ...)
{
  va_list	list;

  if (!r->flag_verbose)
    return;
  va_start(list, msg);
  r_con_vout(r, NULL, msg, list, -1);
  va_end(list);
}

static void
r_con_verboser(R, const char *msg, ...)
{
  va_list	list;

  if (!r->flag_verbose)
    return;
  va_start(list, msg);
  r_con_vout(r, NULL, msg, list, errno);
  va_end(list);
}

static void
r_con_debug(R, const char *msg, ...)
{
  va_list	list;

  if (!r->flag_debug)
    return;
  va_start(list, msg);
  r_con_vout(r, "DEBUG", msg, list, -1);
  va_end(list);
}

static void
r_con_debuger(R, const char *msg, ...)
{
  va_list	list;

  if (!r->flag_debug)
    return;
  va_start(list, msg);
  r_con_vout(r, "DEBUG", msg, list, errno);
  va_end(list);
}

static void
r_con_setup(R, RMODULE)
{
  r->debuger	= r_con_debuger;
  r->debug	= r_con_debug;
  r->verboser	= r_con_verboser;
  r->verbose	= r_con_verbose;
  r->info	= r_con_info;
  r->warn	= r_con_warn;
  r->err	= r_con_err;
}

#endif

