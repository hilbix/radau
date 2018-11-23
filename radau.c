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
#include "tino/getopt.h"

#include "radau_version.h"


#define _STRINGIFY(X)	#X
#define STRINGIFY(X)	_STRINGIFY(X)

#define R_HD_IP_MIN	20
#define R_HD_IP_MAX	60
#define R_HD_UDP	8

#define R_IP4_MIN_SZ	576
#define R_IP6_MIN_SZ	1500
#define R_IP_MAX_SZ	65535

#define R_PORT		19162   /* 0x4adau      */
#define R		struct radau *r


struct radau
  {
#if 0
    char                packet[R_IP_MAX_SZ];
#endif

    void (*	modadd)(R, void (*init)(R), void (*exit)(R));
    int		modules;
    struct
      {
        void (*		init)(R);
        void (*		exit)(R);
      }		module[10];

#define	RADAU_PHASE	1
#include "radau.h"
  };

#define	RADAU_PHASE	2
#include "radau.h"

int
main(int argc, char **argv)
{
  R;
  struct radau	radau;
  int		argn, i;

  argn	= tino_getopt(argc, argv, 0, -1,
                      TINO_GETOPT_VERSION(RADAU_VERSION)
                      " [destination]..",

                      TINO_GETOPT_USAGE
                      "h	this help"
                      ,

                      NULL
                      );
  if (argn<=0)
    return 1;

  r = &radau;
  memset(r, 0, sizeof *r);

#define	RADAU_PHASE	3
#include "radau.h"

  for (; argn<argc; argn++)
    r_addr_add(r, argv[argn]);

  for (i=0; i<r->modules; i++)
    if (r->module[i].init)
      r->module[i].init(r);

  printf("%d addresses\n", r_ring_len(r->ring));

  r_main(r);

  for (i=r->modules; --i>=0; )
    if (r->module[i].exit)
      r->module[i].exit(r);

  return r->code;
}

