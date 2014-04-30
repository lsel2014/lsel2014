#ifndef FBTFT_H
#define FBTFT_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

//Functions 
void put_pixel(int x, int y, short c);
void draw(short c);
char *char_to_bitmap(char a);
void draw_char1(char a, int textX, int textY, short textC);

void draw_line(int nlinea, int color, char *c, int size);
void draw_line_x2(int nlinea, int color, char *c, int size);
void fbtft_init();
void fbtft_destroy();
#endif

