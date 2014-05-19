#include "plugin.h"
#include "interp.h"
#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

typedef void (*setup_func_t) (void);

#define MAX_PLUGINS 100
static struct plugin_ref_t {
	const char* name;
	void* handle;
} plugin[MAX_PLUGINS];
static int n_plugins = 0;

static
int plugin_cmd (char* arg)
{
  if (0 == strcmp(arg, "list")) {
    int i;
    for (i = 0; i < n_plugins; ++i) {
      printf ("%s\n", plugin[i].name);
    }
    return 0;
  }
  if (0 == strncmp(arg, "load ", strlen("load "))) {
    return plugin_load (arg + strlen("load "));
  }
  return 1;
}

int
plugin_load (const char* name)
{
  char cwd[128];
  char fname[256];
  snprintf (fname, sizeof(fname), "%s/%s.so", getcwd (cwd, 128), name);
  char setup_func[128];
  snprintf (setup_func, sizeof(setup_func), "%s_setup", name);

  void* handle = dlopen (fname, RTLD_NOW | RTLD_LOCAL);
  if (!handle) {
    printf ("plugin: %s not found\n", name);
    return 0;
  }
  plugin[n_plugins].name = strdup(name);
  plugin[n_plugins].handle = handle;
  ++n_plugins;

  setup_func_t setup = (setup_func_t) dlsym (handle, setup_func);
  if (setup)
    setup ();

  return n_plugins;
}

void
plugin_setup (void)
{
  interp_addcmd("plugin", plugin_cmd, "load MODULE | list");
}

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
