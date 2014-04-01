/*
 * xmlParser.c
 *
 * Takes the date you have to pass as parameter and downloads
 * the earthtools' XML with astronomical data.
 *
 *  Created on: Mar 13, 2014
 *      Author: enriquecdiez
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFF_INIT_ADDR 50
#define BUFF_ADDR 200


char** xmlParser(char* date){

	char** results;

	FILE *xmlFile;
	char* xmlString;
	long fileSize;

	char *unparsedDate = date;
	char *day = strtok(unparsedDate, "/");
	char *month = strtok(NULL, "/");
	//char *year = strtok(NULL, "/"); No usamos el año pero lo guardo por si acaso

	const char address[BUFF_INIT_ADDR] = "http://www.earthtools.org/sun/40.71417/-74.00639";
	char command[BUFF_ADDR] = "wget -q --output-document=sunrise.xml ";

	strcat(command, address);
	strcat(command, "/");
	strcat(command, day);
	strcat(command, "/");
	strcat(command, month);
	strcat(command, "/");
	strcat(command, "1");
	strcat(command, "/");
	strcat(command, "0");

	system(command);

	xmlFile = fopen("sunrise.xml", "r");
	fseek(xmlFile, 0, SEEK_END);
	fileSize = ftell(xmlFile);
	fseek(xmlFile, 0, SEEK_SET);
	xmlString = malloc(sizeof(char)* fileSize);
	fread(xmlString, 1, fileSize, xmlFile);
	fclose(xmlFile);
	remove("sunrise.xml");

	char* buf1;
	char* buf2;

	if (buf1 = strstr(xmlString, "<sunrise>")){
		char sunriseHour[9];
		sunriseHour[8] = '\0';
		int i;
		char* cursor = strchr(buf1, '>');
		cursor++;
		for (i=0; i<8; i++){
			sunriseHour[i] = cursor[i];
		}
		results[0] = sunriseHour;
		printf("%s\n", sunriseHour);
	}

	if (buf2 = strstr(xmlString, "<sunset>")){
		char sunsetHour[9];
		sunsetHour[8] = '\0';
		int j;
		char* cursor2 = strchr(buf2, '>');
		cursor2++;
		for (j=0; j<8; j++){
			sunsetHour[j] = cursor2[j];
		}
		results[1] = sunsetHour;
		printf("%s\n", sunsetHour);
	}

	return(results);
}

