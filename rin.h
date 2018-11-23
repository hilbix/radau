/* incoming packets
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if	RADAU_PHASE==1

#elif	RADAU_PHASE==2

#include <unistd.h>

static void
r_in(R)
{
}

static void
r_in_init(R)
{
}

static void
r_in_exit(R)
{
}

#elif	RADAU_PHASE==3

r->modadd(r, r_in_init, r_in_exit);

#endif

