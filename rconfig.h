/* Config management
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#if	RADAU_PHASE==1

const char *configdir, *configname;

#elif	RADAU_PHASE==2

#include "tino/dirs.h"

static TINO_DIRS *
r_config_base(R, int write)
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
r_config_path(R, int writer)
{
  /* all is stored in the application subdirectory "radau"	*/
  return tino_dirs_sub(r_config_base(r, writer), 1, r->configdir);
}

static void
r_config_read_wrap(R, void (*reader)(R, FILE *, const char *))
{
  TINO_DIRS	*dirs;
  const char	*name;
  FILE		*fd;

  dirs	= r_config_path(r, 0);
  for (;;)
    {
      if ((name = tino_dirs_path(dirs, 1, r->configname))==0)
        {
          tino_dirs_free(dirs);
          r->info(r, "config not found: %s", r->configname);
          reader(r, NULL, r->configname);
          return;
        }
      if ((fd = fopen(name, "rt"))!=0)
        {
          int	err;

          reader(r, fd, name);
          err	=  ferror(fd);
          err	|= fclose(fd);
          if (err)
            r->warn(r, "error while loading config: %s", name);
          else
            r->info(r, "config loaded: %s", name);
          tino_dirs_free(dirs);
          return;
        }
      tino_dirs_pull(dirs, 1);
      tino_dirs_gc(dirs);
    }
}

static void
r_config_write_wrap(R, void (*writer)(R, FILE *, const char *name))
{
  TINO_DIRS	*dirs;
  const char	*dir, *name1, *name2;
  FILE		*fd;
  int		err;

  dirs	= r_config_path(r, 1);
  dir	= tino_dirs_path(dirs, 0);
  name1	= tino_dirs_path(dirs, 1, r->configname);
  name2	= tino_dirs_path(dirs, 2, r->configname, ".tmp");

  if ((fd = fopen(name2, "w+t"))==0)
    {
      if (mkdir(dir, 0755))
        r->warn(r, "cannot create dir: %s", dir);
      else
        r->info(r, "created dir: %s", dir);
      if ((fd = fopen(name2, "w+t"))==0)
        {
          writer(r, NULL, name2);
          tino_dirs_free(dirs);
          return;
        }
    }
  writer(r, fd, name2);
  err	=  ferror(fd);
  err	|= fclose(fd);
  if (err)
    r->warn(r, "cannot write config: %s", name2);
  else if (rename(name2, name1))
    r->warn(r, "cannot replace config file: %s", name1);
  else
    r->info(r, "config saved to %s", name1);
}

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

