/* Config management
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if	RADAU_PHASE==1

const char *configdir, *configname;

#elif	RADAU_PHASE==2

#include "rconfigdir.h"
#include "rconfigread.h"
#include "rconfigwrite.h"

static void
r_config_read(R)
{
  r_config_read_wrap(r, r_config_read_do);
}

static void
r_config_write(R)
{
  r_config_write_wrap(r, r_config_write_do);
}

static void
r_config_init(R)
{
  r->configdir	= "radau";
  r->configname	= "radau.conf";
  r_config_read(r);
}

static void
r_config_exit(R)
{
  r_config_write(r);
}

#elif	RADAU_PHASE==3

r->modadd(r, r_config_init, r_config_exit);

#endif

