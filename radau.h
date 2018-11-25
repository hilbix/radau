/* This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#define	RADAU_PHASE_CONFIG	1
#define	RADAU_PHASE_CODE	2
#define	RADAU_PHASE_MODULE	3
#define	RADAU_PHASE_GETOPT	4

#include "rmodule.h"

#ifndef	RADAU_MODULE
#define	RADAU_MODULE(X)
#endif

#include "rref.h"

#include "raddr.h"

#include "rtimer.h"

#include "rcon.h"

#include "rpacket.h"
#include "rin.h"
#include "rout.h"

#include "rmain.h"
#include "rconfig.h"

#undef	RADAU_MODULE
#undef	RADAU_PHASE

