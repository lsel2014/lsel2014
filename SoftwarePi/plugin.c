#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>

typedef void (*setup_func_t) (void);

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
  setup_func_t setup = (setup_func_t) dlsym (handle, setup_func);
  if (setup)
    setup ();
  return 1;
}


/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
