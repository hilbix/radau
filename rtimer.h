/* Timing Support
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if	RADAU_PHASE==1

int	tick;
long	delay;

#elif	RADAU_PHASE==2

static struct radau *_r_timer;

static void
r_timer(int x)
{
  if (!_r_timer) return;
  _r_timer->tick++;
}

static void
r_timer_set(R, long usec)
{
  struct timeval	tv;
  struct itimerval	t;

  tv.tv_sec	= usec/1000000;
  tv.tv_usec	= usec%1000000;

  t.it_value	= tv;
  t.it_interval	= tv;

  FATAL(signal(SIGALRM, r_timer)==SIG_ERR);
  FATAL(setitimer(ITIMER_REAL, &t, NULL));
}

static void
r_timer_init(R, RMODULE)
{
  FATAL(_r_timer!=r);
  r_timer_set(r, r->delay);
}

static void
r_timer_exit(R, RMODULE)
{
  FATAL(_r_timer!=r);
  r_timer_set(r, 0l);
}

static void
r_timer_setup(R, RMODULE)
{
  m->init	= r_timer_init;
  m->exit	= r_timer_exit;

  _r_timer	= r;
  r->delay	= 1000000l;
}

#elif	RADAU_PHASE==3

R_MODULE(r_timer);

#endif

