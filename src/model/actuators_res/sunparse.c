#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sunparse.h"
#include "sun.h"

/**
 * xml_find
 *
 * @returns field_content Value of xml field passed as a parameter
 * (must be of exactly 8 characters long)
 */
static
char*
xml_retrieve_field (char* buf, const char* field)
{
	static char field_content[9];
	char* bufAux;

	if ((bufAux = strstr(buf, field)))
	{
		field_content[8] = '\0';
	
	    int i;
		char* cursor = strchr(bufAux, '>');
		cursor++;
		
		for (i=0; i<8; i++)
		{
			field_content[i] = cursor[i];
		}
	}

	return field_content;
}

/*
 * Read file, parse and set sunrise and sunset times
 */
void
sun_read_sunrise_sunset (sun_t* this)
{
    /*
     * Copy file from disk to memory
     */
	char sun_times_buf[30000];
	FILE* f = fopen("sunrise.xml", "r");

	fread (sun_times_buf, 1, 30000, f);
	fclose(f);

 	remove("sunrise.xml");
    

    /* TODO: ADD CHECKS or this WILL blow up*/
   
    /*
     * Set sunrise
     */
    char sunrise_hours, sunrise_minutes, sunrise_seconds;
    
	sunrise_hours = atoi(strtok(xml_retrieve_field (sun_times_buf, "<sunrise>"), ":"));
	sunrise_minutes = atoi(strtok(NULL, ":"));
	sunrise_seconds = atoi(strtok(NULL, ":"));

    sun_time_set_hours (this->sunrise, sunrise_hours);
    sun_time_set_minutes (this->sunrise, sunrise_minutes);
    sun_time_set_seconds (this->sunrise, sunrise_seconds);

    /*
     * Set sunset
     */
    char sunset_hours, sunset_minutes, sunset_seconds;

	sunset_hours = atoi(strtok(xml_retrieve_field (sun_times_buf, "<sunset>"), ":"));
	sunset_minutes = atoi(strtok(NULL, ":"));
	sunset_seconds = atoi(strtok(NULL, ":"));

    sun_time_set_hours (this->sunset, sunset_hours);
    sun_time_set_minutes (this->sunset, sunset_minutes);
    sun_time_set_seconds (this->sunset, sunset_seconds);
}

