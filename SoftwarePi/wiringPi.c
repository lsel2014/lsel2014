#include <wiringPi.h>

static void (*isr[32]) (void);

int
wiringPiSetup (void)
{
  return 0;
}

int
wiringPiISR (int pin, int edgeType, void (*func)(void))
{
  isr[pin] = func;
  return 0;
}

void
pinMode (int pin, int direction)
{
}

void
digitalWrite (int pin, int value)
{
}

void
wiringPi_gen_interrupt (int pin)
{
  if (isr[pin])
    (*isr[pin]) ();
}

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
