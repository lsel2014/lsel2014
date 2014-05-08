#include <stdio.h>
#include <string.h>
#include "model.h"

#define MAX_OBJS 100
static struct object_ref_t {
	const char* name;
	observable_t* obs;
} objs[MAX_OBJS];
static int n_objs = 0;

void
model_setup (void)
{
}

int
model_add (const char* name, observable_t* obs)
{
  if (n_objs >= MAX_OBJS) {
	printf ("model: Too many objects.\n");
	return 0;
  }
  objs[n_objs].name = name;
  objs[n_objs].obs = obs;
  return ++n_objs;
}

observable_t*
model_get_next (const char* name, int* i)
{
  for (; (*i) < n_objs; (*i)++) {
    if (0 == strncmp (name, objs[*i].name, strlen (name)))
      return objs[(*i)++].obs;
  }
  return NULL;
}

observable_t*
model_get (const char* name)
{
  int i = 0;
  return model_get_next (name, &i);
}

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
