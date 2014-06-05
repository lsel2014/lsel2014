#include <bcm2835.h>
#include "i2c1.h"
#include "interp.h"
#define BCM2835_FSEL_INPT 0
#define BCM2835_FSEL_ALT0 4

void 
i2c_pins_setup(void) 
{
bcm2835_init();
bcm2835_gpio_fsel(0,BCM2835_FSEL_INPT);
bcm2835_gpio_fsel(1,BCM2835_FSEL_INPT);
bcm2835_gpio_fsel(2,BCM2835_FSEL_INPT);
bcm2835_gpio_fsel(3,BCM2835_FSEL_INPT);
bcm2835_gpio_fsel(28,BCM2835_FSEL_INPT);
bcm2835_gpio_fsel(29,BCM2835_FSEL_INPT);
// Set bus i2c 0
bcm2835_gpio_fsel(2,BCM2835_FSEL_ALT0);
bcm2835_gpio_set_pud(2,BCM2835_GPIO_PUD_UP);
bcm2835_gpio_fsel(3,BCM2835_FSEL_ALT0);
bcm2835_gpio_set_pud(3,BCM2835_GPIO_PUD_UP);
// Set bus i2c 1
bcm2835_gpio_fsel(28,BCM2835_FSEL_ALT0);
bcm2835_gpio_set_pud(28,BCM2835_GPIO_PUD_UP);
bcm2835_gpio_fsel(29,BCM2835_FSEL_ALT0);
bcm2835_gpio_set_pud(29,BCM2835_GPIO_PUD_UP);

interp_addcmd("i2c", i2c_cmd, "test i2c devices");
}

int i2c_cmd(char* arg)
{
  
	if (0 == strcmp(arg, "test"))
{
  uint16_t i2c0_comand[]={};
  uint16_t i2c1_comand[]={};
  
  uint8_t buff0[],buff1[];
  
  rt_mutex_acquire(&(i2chandler[0]->mutex), TM_INFINITE);
  i2c_send_sequence(i2chandler[0]->i2chandler, i2c0_comand, 8, buff0);
  rt_mutex_release(&i2chandler[0]->mutex);
  
  rt_mutex_acquire(&(i2chandler[1]->mutex), TM_INFINITE);
  i2c_send_sequence(i2chandler[1]->i2chandler, i2c1_comand, 8, buff1);
  rt_mutex_release(&i2chandler[1]->mutex);
}
return 0;
}
	printf(
"Incorrect command. Use train help to see a list of available commands\n");
return 1;
}
