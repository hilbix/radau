/* incoming packets
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if	RADAU_PHASE==1

#elif	RADAU_PHASE==2

static void
r_in(R)
{
}

static void
r_in_setup(R, RMODULE)
{
}

#elif	RADAU_PHASE==3

R_MODULE(r_in);

#endif

