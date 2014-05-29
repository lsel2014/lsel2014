#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <wiringPiI2C.h>
#include "sun.h"
//#include "sunparse.h"
#include "tasks.h"
#include "interp.h"
#include "lsquaredc.h"
#include "daemon.h"

#define I2C_ADRESS_SUN 0x20

sun_t* sun;

void sun_setup(void) {
	sun = sun_new(DEFAULT_DATE, 0x20, SUN_DEADLINE);
	//sun = sun_new(DEFAULT_DATE, I2C_ADRESS_SUN , SUN_DEADLINE);
	interp_addcmd("sun", sun_cmd, "Set sun parameters\n");
}

int sun_cmd(char* arg) {
	if (0 == strncmp(arg, "date ", strlen("date "))) {
		char* date = arg + strlen("date ");
		if (strlen(date) != 10) {
			printf("Incorrect date. Use format dd/mm/yyyy\n");
			return 1;
		}
		sun_set_date(sun, date);
		printf("Sunrise: %d:%d:%d\n", sun->sunrise.hours, sun->sunrise.minutes,
				sun->sunrise.seconds);
		printf("Sunset: %d:%d:%d\n", sun->sunset.hours, sun->sunset.minutes,
				sun->sunset.seconds);
		return 0;
	}
	if (0 == strncmp(arg, "current", strlen("current"))) {
		printf("%d\n", sun->current_simulated_time);
		return 0;
	}

	if (0 == strncmp(arg, "test", strlen("test"))) {
		int i;
		int flag = 1;
		int buffer = -1;
		int x;
		for (i = 0; i < 5; i++) {
			x = sun->current_simulated_time;
			if ((x > buffer) && (x >= 0) && (x < 1201)) {
				buffer = x;
			} else {
				flag = 0;
			}
			sleep(2);
		}
		if (flag == 1) {
			printf("Test Reloj Virtual OK\n");
			return 0;
		} else {
			printf("Test Reloj Virtual NOK\n");
			return 1;
		}

		return 0;
	}

	return 1;
}

void sun_task(void* arg) {
	sun_t* this = (sun_t*) arg;
	rt_task_set_periodic(NULL, TM_NOW, SUN_PERIOD);
	char buffer[9];
	while (1) {
		rt_task_wait_period(NULL);
		sun_update_simulated_time(this);
		if (this->current_simulated_time == 1200) {
			sprintf(buffer, "23:59:59");
		} else {
			int seg_tot, h, r, min, seg;
			seg_tot = (this->current_simulated_time * 86400) / 1200;
			h = seg_tot / 3600;
			r = seg_tot % 3600;
			min = r / 60;
			seg = r % 60;
			sprintf(buffer, "%02d:%02d:%02d", h, min, seg);
		}

		FILE *fp;
		if ((fp = fopen("/var/www/HoraVirtual.txt.new", "w+")) < 0) {
			rt_printf("Error en archivo destino");
		}
		fprintf(fp, "%s", buffer);
		fclose(fp);
		system("mv /var/www/HoraVirtual.txt.new /var/www/HoraVirtual.txt");
	}
}

sun_t*
//sun_new(sun_date_t date, char i2c_address, int deadline)
sun_new(sun_date_t date, uint16_t i2c_address, int deadline) 
{
	sun_t* this = (sun_t*) malloc(sizeof(sun_t));
	sun_init(this, date, i2c_address, deadline);
	return this;
}

void 
//sun_init(sun_t* this, sun_date_t date, char i2c_address, int deadline)
sun_init(sun_t* this, sun_date_t date, uint16_t i2c_address, int deadline)
{
	this->i2c_address = i2c_address;
	//this->i2c_fd = wiringPiI2CSetup(i2c_address);
	sun_set_date(this, date);
	rt_mutex_create(&(this->mutex), "sun_mutex");
	task_add("Sun", deadline, sun_task, this);
}

void 
sun_set_date(sun_t* this, sun_date_t date) 
{
    
        uint16_t sun_comand[21]={(this->i2c_address<<1), 0xFF, I2C_RESTART,
                                 (this->i2c_address<<1), 0x00, I2C_RESTART,
                                 (this->i2c_address<<1), 0x00, I2C_RESTART,
                                 (this->i2c_address<<1), 0x00, I2C_RESTART,
                                 (this->i2c_address<<1), 0x00, I2C_RESTART,
                                 (this->i2c_address<<1), 0x00, I2C_RESTART,
                                 (this->i2c_address<<1), 0x00, I2C_RESTART};
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	this->date = date;
	rt_mutex_release(&(this->mutex));

//if (sun_date_cmp (date, current_date) != 0) {
	char cmd[1024];
	char aux[6];
	char *unparsedDate = strdup(this->date);
	char *day = strtok(unparsedDate, "/");
	char *month = strtok(NULL, "/");
	snprintf(aux, 6, "%s/%s", day, month);
	snprintf(cmd, 1024, "wget -q --output-document=sunrise.xml "
			"http://www.earthtools.org/sun/40.4521/-3.7266/%s/1/0", aux);
	system(cmd);
	sun_parse_data(this);
	/*wiringPiI2CWrite(this->i2c_fd, 0xFF);
	wiringPiI2CWrite(this->i2c_fd, this->sunrise.hours);
	wiringPiI2CWrite(this->i2c_fd, this->sunrise.minutes);
	wiringPiI2CWrite(this->i2c_fd, this->sunrise.seconds);
	wiringPiI2CWrite(this->i2c_fd, this->sunset.hours);
	wiringPiI2CWrite(this->i2c_fd, this->sunset.minutes);
	wiringPiI2CWrite(this->i2c_fd, this->sunset.seconds);*/
	sun_comand[4]=this->sunrise.hours;
	sun_comand[7]=this->sunrise.minutes;
	sun_comand[10]=this->sunrise.seconds;
	sun_comand[13]=this->sunset.hours;
	sun_comand[16]=this->sunset.minutes;
	sun_comand[19]=this->sunset.seconds;
	
	//rt_mutex_acquire(&(i2chandler[1]->mutex), TM_INFINITE);
        //i2c_send_sequence(i2chandler[1]->i2chandler, sun_comand, 21, 0);
	//rt_mutex_release(&i2chandler[1]->mutex);
	
}

void 
sun_update_simulated_time(sun_t* this) 
{
	//wiringPiI2CWrite(this->i2c_fd, 0xFE);
	uint16_t sun_read_comand[]={(this->i2c_address<<1), 0xFE, I2C_RESTART,
                                    (this->i2c_address<<1)|1, I2C_READ, I2C_READ};                 
	uint8_t* buff = (uint8_t*) malloc(sizeof(uint8_t)*2);
	
	//char byte1,byte0;
	//char byte1 = wiringPiI2CRead(this->i2c_fd);
	//char byte0 = wiringPiI2CRead(this->i2c_fd);
	//rt_mutex_acquire(&(i2chandler[1]->mutex), TM_INFINITE);
        //i2c_send_sequence(i2chandler[1]->i2chandler, sun_read_comand, 6, buff);
	//rt_mutex_release(&i2chandler[1]->mutex);
	
	rt_mutex_acquire(&(this->mutex), TM_INFINITE);
	//this->current_simulated_time = (byte0 << 8) + byte1;
	
	this->current_simulated_time = (buff[0] << 8) + buff[1];
	rt_mutex_release(&(this->mutex));
}

int 
sun_get_simulated_time(sun_t* this) 
{
	return this->current_simulated_time;
}
;

void 
sun_destroy(sun_t* this) 
{
	free(this);
}
char*
xml_find(char* buf, const char* tag) {
	static char hour[9];
	char* bufAux;
	if ((bufAux = strstr(buf, tag))) {
		hour[8] = '\0';
		int i;
		char* cursor = strchr(bufAux, '>');
		cursor++;
		for (i = 0; i < 8; i++) {
			hour[i] = cursor[i];
		}
	}
	return hour;
}

void 
sun_parse_data(sun_t* this) 
{
	char buf[30000];
	FILE* f = fopen("sunrise.xml", "r");
	fread(buf, 1, 30000, f);
	fclose(f);
	remove("sunrise.xml");
	/* TODO: ADD CHECKS or this WILL blow up*/
	this->sunrise.hours = atoi(strtok(xml_find(buf, "<sunrise>"), ":"));
	this->sunrise.minutes = atoi(strtok(NULL, ":"));
	this->sunrise.seconds = atoi(strtok(NULL, ":"));
	this->sunset.hours = atoi(strtok(xml_find(buf, "<sunset>"), ":"));
	this->sunset.minutes = atoi(strtok(NULL, ":"));
	this->sunset.seconds = atoi(strtok(NULL, ":"));
}
