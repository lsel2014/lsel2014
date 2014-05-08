#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <wiringPi.h>

#include "task.h"
#include "interp.h"
#include "model.h"
#include "plugin.h"

#ifdef __XENO__
#include <rtdk.h>
#endif

// Dummy function to catch signals
void catch_signal() { }

void
initializeXenomaiEnv (void)
{
	// Catch signals
	signal(SIGTERM, catch_signal);
	signal(SIGINT, catch_signal);
#ifdef __XENO__
	/* Avoids memory swapping for this program */
	mlockall(MCL_CURRENT | MCL_FUTURE);
	// Initialize rdtk to use rt_printf
	rt_print_auto_init(1);
#endif
}

void
initializeWiringPi (void)
{
	wiringPiSetup();

	// Replace with proper I2C modules load on kernel
	//system("gpio load i2c");

	//pinModes ....
}

static
void
load_config_file (const char* fname)
{
  FILE* f = fopen (fname, "r");
  char p[128];
  while (fscanf (f, "%s", p) > 0) {
    plugin_load (p);
  }
  fclose (f);
}

int
main (int argc, char* argv[])
{
  // Initialize Xenomai RT enviroment
  initializeXenomaiEnv();

  // Initialize wiringPi lib, configure IO
  initializeWiringPi();

  task_setup ();
  model_setup ();

  if (argc > 1)
    load_config_file (argv[1]);

  task_setup_priorities ();
  interp_run ();

  task_delete_all ();
  return 0;
}

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
