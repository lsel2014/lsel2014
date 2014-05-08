#ifndef FBTFT_H
#define FBTFT_H

struct screen_t;
typedef struct screen_t screen_t;

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


/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
