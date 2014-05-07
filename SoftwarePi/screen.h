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
#include "train.h"


typedef struct {
    observer_t observer;
    int fbfd;
    char* fb_pointer;
    int bpp;
    char* dev;
    train_t* t[MAXTRAINS];
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo orig_vinfo;
}screen_t;

//Functions 
void screen_setup();
void put_pixel(screen_t* this,int x, int y, short c);
void draw(screen_t* this,short c);
char *char_to_bitmap(char a);
void draw_char1(screen_t* this,char a, int textX, int textY, short textC);
void draw_line(screen_t* this,int nlinea, int color, char *c, int size);
void draw_line_x2(screen_t* this,int nlinea, int color, char *c, int size);
screen_t* screen_new(int bpp, char* dev);
void screen_init(screen_t* this, int bpp, char* dev);
void screen_destroy();
#endif

