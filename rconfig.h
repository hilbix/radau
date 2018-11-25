/* Config management
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

RADAU_MODULE(config)

#if	RADAU_PHASE==RADAU_PHASE_CONFIG

const char	*configdir, *configname, *configuser;

#elif	RADAU_PHASE==RADAU_PHASE_CODE

#include "rconfigdir.h"

static void
r_config_init(R, RMODULE)
{
  r->configdir	= "radau";
  r->configname	= "radau.conf";
  r->configuser	= ".local";
  r_config_read(r);
  r_config_write(r);
}

static void
r_config_exit(R, RMODULE)
{
  r_config_write(r);
}

static void
r_config_setup(R, RMODULE)
{
  m->init	= r_config_init;
  m->exit	= r_config_exit;
}

#endif

