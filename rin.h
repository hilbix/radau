/* incoming packets
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

RADAU_MODULE(in)

#if	RADAU_PHASE==RADAU_PHASE_CONFIG

#elif	RADAU_PHASE==RADAU_PHASE_CODE

static void
r_in(R)
{
  pause();
}

static void
r_in_setup(R, RMODULE)
{
}

#endif

