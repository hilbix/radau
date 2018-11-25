/* incoming packets
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if	RADAU_PHASE==1

int main_returncode;

#elif	RADAU_PHASE==2

static void
r_main(R)
{
  r->info(r, "%d addresses", r_ring_len(r->ring));
  while (r->sock>=0)                                                                                
    {
      r_in(r);
      r_out(r);
    }
}

static void
r_main_setup(R, RMODULE)
{
}

#elif	RADAU_PHASE==3

R_MODULE(r_main);

#endif

