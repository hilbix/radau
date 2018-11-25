/* Config directory handling
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

struct rconfig_write
  {
    FILE	*fd;
    const char	*name;
  };

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

static int
r_config_read_try(R, const char *name)
{
  FILE	*fd;
  int	err;

  if (!name || (fd = fopen(name, "rt"))==0)
    {
      r->debuger(r, "config %s", name);
      return 0;
    }

#if 0
  reader(r, fd, name);
#endif

  err	=  ferror(fd);
  err	|= fclose(fd);
  if (err)
    return r->err(r, "reading config %s", name), -1;
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
r_config_write(R)
{
  TINO_DIRS	*dirs;
  const char	*dir, *name1, *name2;
  FILE		*fd;
  int		err;
  struct rconfig_write	c;

  dirs	= r_config_path(r, 1);
  dir	= tino_dirs_path(dirs, 0);
  name1	= tino_dirs_path(dirs, 1, r->configname);
  name2	= tino_dirs_path(dirs, 2, r->configname, ".tmp");

  if ((fd = fopen(name2, "w+t"))==0)
    {
      if (mkdir(dir, 0755))
        r->err(r, "cannot create dir %s", dir);
      else
        r->verbose(r, "created dir %s", dir);
      if ((fd = fopen(name2, "w+t"))==0)
        {
          r->err(r, "cannot write config %s", name2);
          tino_dirs_free(dirs);
          return;
        }
    }

  c.name	= name2;
  c.fd		= fd;
  r_module_config_write(r, &c);

  err	=  ferror(fd);
  err	|= fclose(fd);
  if (err)
    r->warn(r, "cannot write config: %s", name2);
  else if (rename(name2, name1))
    r->warn(r, "cannot replace config file: %s", name1);
  else
    r->info(r, "config saved to %s", name1);
}

