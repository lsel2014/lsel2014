#include <bcm2835.h>
#include "i2c1.h"
#include "interp.h"
#include "daemon.h"
#include "lsquaredc.h"
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
	  uint16_t init_sequence1[] = {0x20<<1};
	  uint16_t init_sequence2[] = {0x20<<1};
	  uint16_t pn_query[] = {0x20<<1, 0x8a, I2C_RESTART, (0x20<<1)|1, I2C_READ};
	  uint8_t status;
	  int result;
	
	  printf("Opened bus, result=%d\n", (i2chandler[0]->i2chandler) );
	  rt_mutex_acquire(&(i2chandler[0]->mutex), TM_INFINITE);
	  result = i2c_send_sequence(i2chandler[0]->i2chandler , init_sequence1, 1, 0);
	  rt_mutex_release(&i2chandler[0]->mutex);
	  printf("Sequence processed, result=%d\n", result);
	  rt_mutex_acquire(&(i2chandler[0]->mutex), TM_INFINITE);
	  result = i2c_send_sequence(i2chandler[0]->i2chandler , init_sequence2, 1, 0);
	  rt_mutex_release(&i2chandler[0]->mutex);
	  printf("Sequence processed, result=%d\n", result);
	  rt_mutex_acquire(&(i2chandler[0]->mutex), TM_INFINITE);
	  result = i2c_send_sequence(i2chandler[0]->i2chandler , pn_query, 5, &status);
	  rt_mutex_release(&i2chandler[0]->mutex);
	  printf("Sequence processed, result=%d\n", result);
	  printf("Status=%d\n", (int)(status));
	  
	  printf("Opened bus, result=%d\n", (i2chandler[1]->i2chandler) );
	  rt_mutex_acquire(&(i2chandler[1]->mutex), TM_INFINITE);
	  result = i2c_send_sequence(i2chandler[1]->i2chandler , init_sequence1, 1, 0);
	  rt_mutex_release(&i2chandler[1]->mutex);
	  printf("Sequence processed, result=%d\n", result);
	  rt_mutex_acquire(&(i2chandler[1]->mutex), TM_INFINITE);
	  result = i2c_send_sequence(i2chandler[1]->i2chandler , init_sequence2, 1, 0);
	  rt_mutex_release(&i2chandler[1]->mutex);
	  printf("Sequence processed, result=%d\n", result);
	  rt_mutex_acquire(&(i2chandler[1]->mutex), TM_INFINITE);
	  result = i2c_send_sequence(i2chandler[1]->i2chandler , pn_query, 5, &status);
	  rt_mutex_release(&i2chandler[1]->mutex);
	  printf("Sequence processed, result=%d\n", result);
	  printf("Status=%d\n", (int)(status));
	  return 0;
	}

	
	printf("Incorrect command. Use train help to see a list of available commands\n");
	return 1;
}
