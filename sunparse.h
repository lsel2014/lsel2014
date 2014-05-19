#ifndef SUNPARSE_H
#define SUNPARSE_H

#include "sun.h"

char* xml_find(char* buf, const char* tag);
void sun_parse_data(sun_t* this);

#endif
