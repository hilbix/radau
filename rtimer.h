/* Timing	*/

#include <signal.h>
#include <sys/time.h>

static struct radau *_r;

static void
r_timer(int x)
{
  if (!_r) return;
  _r->tick++;
}

void
r_timer_start(R)
{
  struct timeval	tv;
  struct itimerval	t;

  _r		= r;

  tv.tv_sec	= 1;
  tv.tv_usec	= 0;

  t.it_value	= tv;
  t.it_interval	= tv;

  FATAL(signal(SIGALRM, r_timer)==SIG_ERR);
  FATAL(setitimer(ITIMER_REAL, &t, NULL));
}

