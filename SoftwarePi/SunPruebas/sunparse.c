#include "sunparse.h"

static
char*
xml_find (char* buf, const char* tag)
{
	static char hour[9];
	char* bufAux;
	if ((bufAux = strstr(buf, tag))){
		hour[8] = '\0';
		int i;
		char* cursor = strchr(bufAux, '>');
		cursor++;
		for (i=0; i<8; i++){
			hour[i] = cursor[i];
		}
		//printf("%s\n", hour);
	}
	return hour;
}

void
sun_parse_data (sun_t* this)
{
	char buf[30000];
	FILE* f = fopen("sunrise.xml", "r");
	fread(buf, 1, 30000, f);
	fclose(f);
	remove("sunrise.xml");

	if (this->sunrise) free ((char*)this->sunrise);
	this->sunrise = strdup (xml_find (buf, "<sunrise>"));

	if (this->sunset) free ((char*)this->sunset);
	this->sunset = strdup (xml_find (buf, "<sunset>"));
}
