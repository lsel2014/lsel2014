/*
 * main.c
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


int main(int argc, char** argv){

	char *unparsedDate = argv[1];
	char *day = strtok(unparsedDate, "/");
	char *month = strtok(NULL, "/");
	char *year = strtok(NULL, "/");

	char address[BUFF_INIT_ADDR] = "http://www.earthtools.org/sun/40.71417/-74.00639";
	char command[BUFF_ADDR] = "wget --output-document=sunrise.xml ";

	strcat(command, address);
	strcat(command, "/");
	strcat(command, day);
	strcat(command, "/");
	strcat(command, month);
	strcat(command, "/");
	strcat(command, "1");
	strcat(command, "/");
	strcat(command, "0");
	//printf("Ejecutando: %s", command);

	system(command);
	return 0;
}

