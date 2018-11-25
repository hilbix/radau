/* RADAU wrapper
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#define _GNU_SOURCE

#include "tino/dirty.h"

#include "tino/file.h"

#include "tino/alloc.h"
#include "tino/fatal.h"

#include "tino/strprintf.h"
#include "tino/buf_line.h"
#include "tino/buf_quote.h"

#include "tino/dirs.h"

#include "tino/getopt.h"

#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "radau_version.h"

#define _STRINGIFY(X)	#X
#define STRINGIFY(X)	_STRINGIFY(X)

#define R_PORT		19162   /* 0x4adau      */
#define R		struct radau *r

struct radau;

#define	RADAU_PHASE	0
#include "radau.h"

struct radau
  {
#define	RADAU_PHASE	RADAU_PHASE_CONFIG
#include "radau.h"
  };

#define	RADAU_PHASE	RADAU_PHASE_CODE
#include "radau.h"

int
main(int argc, char **argv)
{
  R;
  struct radau	radau;
  int		argn;

  r = &radau;
  memset(r, 0, sizeof *r);

#define	RADAU_PHASE	RADAU_PHASE_MODULE
#include "radau.h"

  argn	= tino_getopt(argc, argv, 0, -1,
                      TINO_GETOPT_VERSION(RADAU_VERSION)
                      " [destination]..",

                      TINO_GETOPT_USAGE
                      "h	this help"

#define	RADAU_PHASE	RADAU_PHASE_GETOPT
#include "radau.h"

                      , NULL
                      );
  if (argn<=0)
    return 1;

  r_module_setup(r);

  for (; argn<argc; argn++)
    r_addr_add(r, argv[argn]);

  r_module_init(r);
  r_main(r);
  r_module_exit(r);

  return r->main_returncode;
}

