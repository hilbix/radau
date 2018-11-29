/* incoming packets
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

RADAU_MODULE(main)

#if	RADAU_PHASE==RADAU_PHASE_CONFIG

#define	R_MAIN_MAXTMP	1000

int main_returncode;
int		tmp_count;
const char *	_tmp[R_MAIN_MAXTMP];
const char *(*	tmp)(R, const char *);

#elif	RADAU_PHASE==RADAU_PHASE_CODE

static const char *
r_main_tmp(R, const char *s)
{
  FATAL(r->tmp_count >= R_MAIN_MAXTMP);
  r->_tmp[r->tmp_count++]	= s;
  return s;
}

static void
r_main_cleanup(R)
{
  while (r->tmp_count>0)
    tino_free_constO(r->_tmp[--r->tmp_count]);
}

static volatile int	r_main_loop;

static void
r_main_terminate()
{
  r_main_loop	= 0;
}

static void
r_main_term(int sig)
{
  tino_sigset(sig, r_main_terminate);
  tino_sigfix(sig);
}

static void
r_main(R)
{
  r->info(r, "%d addresses", r_ring_len(r->ring));
  tino_sigintr(SIGALRM);
  r_main_loop	= 1;
  r_main_term(SIGHUP);
  r_main_term(SIGINT);
  r_main_term(SIGQUIT);
  r_main_term(SIGTERM);
  while (r->sock>=0 && r_main_loop)                                                                                
    {
      r_in(r);
      if (r->sock>=0)
        r_out(r);
      r_main_cleanup(r);
    }
}

static void
r_main_setup(R, RMODULE)
{
  r->tmp	= r_main_tmp;
}

#endif

