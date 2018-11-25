/* RADAU modules
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if	RADAU_PHASE==1

#elif	RADAU_PHASE==2

#define	RMODULE	struct rmodule *m

#define	R_MODULE_MAX	10
#define	MOD		_lkdfadkasldkp23sde0923_

struct rmodule;

typedef	void rmodule_fn(R, RMODULE);

struct rmodule
  {
    rmodule_fn	*setup, *init, *exit;
  };

static struct MOD
  {
    int			cnt;
    struct rmodule	data[R_MODULE_MAX];
  } MOD;

static void
r_module(R, rmodule_fn setup)
{
  FATAL(MOD.cnt >= R_MODULE_MAX);
  MOD.data[MOD.cnt++].setup	= setup;
}

static void
r_module_setup(R)
{
  RMODULE;
  int		i;

  for (m=MOD.data, i=MOD.cnt; --i>=0; m++)
    m->setup(r, m);
}

static void
r_module_init(R)
{
  RMODULE;
  int		i;

  for (m=MOD.data, i=MOD.cnt; --i>=0; m++)
    m->init(r, m);
}

static void
r_module_exit(R)
{
  RMODULE;
  int		i;

  for (m=MOD.data, i=MOD.cnt; --i>=0; m++)
    m->exit(r, m);
}

#undef	MOD

#elif	RADAU_PHASE==3

#define	R_MODULE(name)	r_module(r, name ## _ ## setup)

#if 0
  RMODULE(r_module);
#endif

#endif

