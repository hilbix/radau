/* RADAU modules
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if 0
  R_MODULE(r_module)
#endif

#if	RADAU_PHASE==RADAU_PHASE_CONFIG

#elif	RADAU_PHASE==RADAU_PHASE_CODE

#define	RMODULE	struct rmodule *m

#define	R_MODULE_MAX	10
#define	MOD		_lkdfadkasldkp23sde0923_

struct rmodule;
struct rconfig_write;

typedef	void rmodule_fn(R, RMODULE);

struct rmodule
  {
    const char	*name;
    rmodule_fn	*setup, *init, *exit;
    int		(*read)(R, const char *key, const char *val);
    void	(*write)(R, struct rconfig_write *);
  };

static struct MOD
  {
    int			cnt;
    struct rmodule	data[R_MODULE_MAX];
  } MOD;

static void
r_module(R, const char *name, rmodule_fn setup)
{
  RMODULE;

  FATAL(MOD.cnt >= R_MODULE_MAX);

  m		= &MOD.data[MOD.cnt++];
  m->name	= name;
  m->setup	= setup;
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
    if (m->init)
      m->init(r, m);
}

static void
r_module_exit(R)
{
  RMODULE;
  int		i;

  /* backwards	*/
  for (i=MOD.cnt; --i>=0; )
    if ((m = &MOD.data[i])->exit)
      m->exit(r, m);
}

static int
r_module_config_read(R, const char *module, const char *key, const char *value)
{
  RMODULE;
  int	i;

  for (m=MOD.data, i=MOD.cnt; --i>=0; m++)
    if (!strcmp(m->name, module))
      return m->read ? m->read(r, key, value) : 1;
  return -1;
}

static void
r_module_config_write(R, struct rconfig_write *c)
{
  RMODULE;
  int	i;

  for (m=MOD.data, i=MOD.cnt; --i>=0; m++)
    if (m->write)
      m->write(r, c);
}

#undef	MOD

#elif	RADAU_PHASE==RADAU_PHASE_MODULE

#define	RADAU_MODULE(name)	r_module(r, #name, r_ ## name ## _ ## setup);

#endif

