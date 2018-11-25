/* Config management
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

RADAU_MODULE(config)

#if	RADAU_PHASE==0

struct rconfig_read
  {
    struct radau *r;
    const char	*name, *module, *key, *val;
    void	(*read)(struct rconfig_read *, const char *key, int (*)(struct radau *, const char *));
    int		bug, err;
  };

struct rconfig_write
  {
    TINO_BUF	buf;
    struct radau *r;
    const char	*name, *module;
    void	(*write)(struct rconfig_write *, const char *key, const char *val);
  };

#elif	RADAU_PHASE==RADAU_PHASE_CONFIG

const char	*configdir, *configname, *configuser;

#elif	RADAU_PHASE==RADAU_PHASE_CODE

static TINO_DIRS *
r_config_dirs(R, int write)
{
  TINO_DIRS	*dirs;

  dirs	= tino_dirs_new();

  /* Define the local context.	*/
  tino_dirs_add(dirs, 1, getenv("XDG_CONFIG_HOME"));
  tino_dirs_add(dirs, 2, getenv("HOME"), ".config");
  tino_dirs_prune(dirs, 1);

  /* We cannot write in global directories	*/
  if (write)
    return dirs;

  TINO_DIRS	*tmp;

  /* Append the global context for readers.	*/
  tmp	= tino_dirs_new();
  tino_dirs_add(tmp, 1, getenv("XDG_CONFIG_DIRS"));
  tino_dirs_add(tmp, 1, "/etc/xdg");
  tino_dirs_prune(tmp, 1);

  /* This can be a pathvar, so exand it	before adding	*/
  tino_dirs_expand_pathvar(tmp);

  /* Add all to the stack	*/
  tino_dirs_append(dirs, tmp);
  tino_dirs_free(tmp);

  return dirs;
}

static TINO_DIRS *
r_config_path(R, int write)
{
  /* all is stored in the application subdirectory "radau"	*/
  return tino_dirs_sub(r_config_dirs(r, write), 1, r->configdir);
}

static void
r_config_reader(struct rconfig_read *c, const char *key, int (*fn)(struct radau *, const char *))
{
  if (!strcmp(c->key, key))
    c->bug	= fn(c->r, c->val);
}

static int
r_config_read_try(R, const char *name)
{
  TINO_BUF		buf1, buf2;
  struct rconfig_read	cr;
  int			fd, nr;
  const char		*line;

  xDP(("(%s)", name));
  if (!name || (fd = open(name, O_RDONLY))<0)
    {
      r->debuger(r, "config %s", name);
      return 0;
    }

  cr.r		= r;
  cr.read	= r_config_reader;
  cr.err	= 0;

  tino_buf_initO(&buf1);
  tino_buf_initO(&buf2);
  for (nr=1; (line = tino_buf_line_readE(&buf1, fd, '\n'))!=0; nr++)
    {
      const char	*mod, *key, *val;
      int		c;

      tino_buf_freeO(&buf2);
      tino_buf_add_sO(&buf2, line);
      DP(("() line %s", line));
      if ((c = tino_buf_trimO(&buf2))<0 || c=='#')
        continue;
      DP(("() ok"));

      mod	= tino_buf_unquoteN(&buf2, '.');
      key	= tino_buf_unquoteN(&buf2, '=');
      val	= tino_buf_unquoteN(&buf2, 0);

      if (!mod || !key || !val || ((c = tino_buf_trimO(&buf2))>=0 && c!='#'))
        {
          r->err(r, "%s:%d: config file format error", name, nr);
          break;
        }

      cr.module	= mod;
      cr.key	= key;
      cr.val	= val;
      cr.bug	= -1;

      DP((" %s %s %s", mod, key, val));
      r_module_config_read(r, &cr);

      if (cr.bug && ++cr.err<3)
        r->warn(r, "%s:%d: unknown configuration", name, nr);
    }
  if (tino_file_closeE(fd) || errno)
    return r->err(r, "reading config %s", name), -1;
  if (cr.err)
    return r->err(r, "%d error(s)", cr.err), -1;
  return r->info(r, "loaded config %s", name), 1;
}

static void
r_config_read(R)
{
  TINO_DIRS	*dirs;
  const char	*name;

  dirs	= r_config_path(r, 0);
  for (;;)
    {
      int	got;

      if ((name = tino_dirs_path(dirs, 1, r->configname))==0)
        {
          tino_dirs_free(dirs);
          r->info(r, "config not found: %s", r->configname);
          return;
        }
      if ((got = r_config_read_try(r, name)) != 0)
        {
          if (got>0)
            r_config_read_try(r, tino_dirs_path(dirs, 2, r->configname, r->configuser));
          tino_dirs_free(dirs);
          return;
        }

      tino_dirs_pull(dirs, 1);
      tino_dirs_gc(dirs);
    }
}

static void
r_config_writer(struct rconfig_write *c, const char *key, const char *val)
{
  xDP((" '%s' '%s' '%s'", c->module, key, val));
  tino_buf_add_sO(&c->buf, c->module);
  tino_buf_add_cO(&c->buf, '.');
  tino_buf_add_sO(&c->buf, key);
  tino_buf_add_cO(&c->buf, '=');
  tino_buf_quoteN(&c->buf, val);
  tino_buf_add_cO(&c->buf, '\n');
}

static void
r_config_write(R)
{
  TINO_DIRS	*dirs;
  const char	*dir, *name1, *name2;
  int		fd, err;
  struct rconfig_write	c;

  dirs	= r_config_path(r, 1);
  dir	= tino_dirs_path(dirs, 0);
  name1	= tino_dirs_path(dirs, 1, r->configname);
  name2	= tino_dirs_path(dirs, 2, r->configname, ".tmp");

  if ((fd = open(name2, O_CREAT|O_WRONLY, 0644))<0)
    {
      if (mkdir(dir, 0755))
        r->err(r, "cannot create dir %s", dir);
      else
        r->verbose(r, "created dir %s", dir);
      if ((fd = open(name2, O_CREAT|O_WRONLY, 0644))<0)
        {
          r->err(r, "cannot write config %s", name2);
          tino_dirs_free(dirs);
          return;
        }
    }

  tino_buf_initO(&c.buf);

  c.r		= r;
  c.write	= r_config_writer;
  c.name	= name2;
  r_module_config_write(r, &c);

  err	= tino_buf_write_all_1E(&c.buf, fd);
  tino_buf_freeO(&c.buf);

  if (tino_file_closeE(fd) || err)
    r->warn(r, "cannot write config: %s", name2);
  else if (rename(name2, name1))
    r->warn(r, "cannot replace config file: %s", name1);
  else
    r->info(r, "config saved to %s", name1);
}


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

