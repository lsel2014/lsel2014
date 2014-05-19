#include "sunparse.h"
#include "sun.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static
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

void sun_parse_data(sun_t* this) {
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
