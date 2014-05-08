#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "train.h"
#include "screen.h"
#include "train.h"
#include "model.h"


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
} screen_t;

screen_t* mini_screen;
static void screen_notify(observer_t* this, observable_t* foo);

// 'global' variables to store screen info

//Characters.

char A[] = { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
char R[] = { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0,
		0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
char N[] = { 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0,
		1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
char S[] = { 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
char L[] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
		0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
char B[] = { 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char C[] = { // C
		0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0,
				0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char D[] = { // D
		0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0,
				0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0,
				1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char E[] = { // E
		0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
				0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char F[] = { // F
		0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
				0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char T[] = { 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char I[] = { 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char M[] = { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1,
		0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
		0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char Z[] = { // Z
		0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char espacio[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char comillas[] = { 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0,
		0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char point[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char doublepoint[] = { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char zero[] = { 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1,
		0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0,
		0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char one[] = { // 1
		0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
				0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
				0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char two[] = { // 2
		0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
				0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char three[] = { // 8
		0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0,
				0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char four[] = {  // 9
		0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0,
				0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
				0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char five[] = { // :
		0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
				0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
				0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char six[] = { // :
		0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
				0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0,
				0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char seven[] = { // :
		0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
				0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
				0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char eight[] = { // :
		0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0,
				0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

char nine[] = { // :
		0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0,
				0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
				0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

void screen_setup(){
	mini_screen=screen_new(16,"/dev/fb1");
	train_t* t;
	for (t=mini_screen->t[0]; t->name; ++t) {
			observable_t* obs = model_get_observable(t->name);
			observable_register_observer(obs, &mini_screen->observer);
	}
}

screen_t* screen_new(int bpp, char* dev) {
	screen_t* this = (screen_t*) malloc(sizeof(screen_t));
	screen_init(this, bpp, dev);
	return this;
}

void screen_init(screen_t* this, int bpp, char* dev) {
	int i;
	observer_init((observer_t*) this, screen_notify);
	this->t[0] = (train_t*) model_get_observable("Diesel");
	this->t[1] = (train_t*) model_get_observable("Renfe");
	this->t[2] = NULL;
	this->dev = dev;
	this->bpp = bpp;
	this->fbfd = open(dev, O_RDWR);
	if (!this->fbfd) {
		printf("Error: cannot open framebuffer device.\n");
	}
	//printf("The framebuffer device was opened successfully.\n");

	// Get variable screen information
	if (ioctl(this->fbfd, FBIOGET_VSCREENINFO, &this->vinfo)) {
		printf("Error reading variable information.\n");
	}
	/*printf("Original %dx%d, %dbpp\n", vinfo.xres, vinfo.yres,
	 vinfo.bits_per_pixel);*/

	// Store for reset (copy vinfo to vinfo_orig)
	memcpy(&this->orig_vinfo, &this->vinfo, sizeof(struct fb_var_screeninfo));

	// Change variable info
	this->vinfo.bits_per_pixel = bpp;
	if (ioctl(this->fbfd, FBIOPUT_VSCREENINFO, &this->vinfo)) {
		printf("Error setting variable information.\n");
	}

	// Get fixed screen information
	if (ioctl(this->fbfd, FBIOGET_FSCREENINFO, &this->finfo)) {
		printf("Error reading fixed information.\n");
	}
	// map fb to user mem
	unsigned long screensize = this->finfo.smem_len;
	this->fb_pointer = (char*) mmap(0, screensize, PROT_READ | PROT_WRITE,
			MAP_SHARED, this->fbfd, 0);

	if ((int) this->fb_pointer == -1) {
		printf("Failed to mmap.\n");
	}
}

static
void screen_notify(observer_t* this, observable_t* foo) {
	screen_t* s = (screen_t*)this;
	int i;
	char line[20];
	draw(s, 0x1818); //Background
	float est;
	int nsect;
	for (i = 0; s->t[i]; ++i) {
		train_t* t = s->t[i];
		est = train_get_current_time_estimation(t);
		nsect = train_get_sector(t);
		if (train_get_direction(t) == FORWARD) {
			nsect = (nsect + 1) % 4;
		} else {
			nsect = (nsect > 0) ? nsect - 1 : 3;
		}
		sprintf(line, "%6s %d %2.2f", train_get_name(t), nsect, est);
		draw_line(s, i + 2, 0xFFFF, line, 20);
	}
}

// helper function to 'plot' a pixel in given color
void put_pixel(screen_t* this, int x, int y, short c) {
	// calculate the pixel's byte offset inside the buffer
	unsigned int pix_offset = x + y * this->finfo.line_length;

	char upper = (c >> 8) & 0xFF;
	char lower = c & 0xFF;

	// now this is about the same as 'fbp[pix_offset] = value'
	*((char*) (this->fb_pointer + pix_offset)) = upper;
	*((char*) (this->fb_pointer + pix_offset + 1)) = lower;

}

// helper function for drawing - no more need to go mess with
// the main function when just want to change what to draw...

void draw(screen_t* this, short c) {

	int x, y;

	for (y = 0; y < this->vinfo.yres - 1; y++) {
		for (x = 0; x < this->vinfo.xres * 2; x++) {

			// color based on the 16th of the screen width
			// short c = 0x1818;

			// call the helper function

			put_pixel(this, x, y, c);

		}
	}

}

void draw_char(screen_t* this, char *a, int textX, int textY, short textC) {
	int y = 0;
	int x = 0;
	// loop through pixel rows
	for (y = 0; y < 8; y++) {
		// loop through pixel columns
		for (x = 0; x < 8; x++) {
			// get the pixel value
			char b = a[y * 8 + x];
			if (b > 0) { // plot the pixel
				put_pixel(this, textX + (x * 2), textY + y, textC);
			} else {
				// leave empty (or maybe plot 'text backgr color')
			}
		} // end "for x"
	} // end "for y"

}

// Function to draw a char bigger.
void draw_char_x2(screen_t* this, char a, int textX, int textY, int textC) {
	int y = 0;
	int x = 0;
	char *character = char_to_bitmap(a);
// loop through pixel rows
	for (y = 0; y < 8; y++) {
		// loop through pixel columns
		for (x = 0; x < 8; x++) {
			// get the pixel value
			char b = character[y * 8 + x];
			if (b > 0) { // plot the pixel
				put_pixel(this, textX + (x * 4), textY + (y * 2), textC);
				put_pixel(this, textX + (x * 4) + 2, textY + (y * 2), textC);
				put_pixel(this, textX + (x * 4), textY + (y * 2) + 1, textC);
				put_pixel(this, textX + (x * 4) + 2, textY + (y * 2) + 1,
						textC);
			} else {
				// leave empty (or maybe plot 'text backgr color')
			}
		} // end "for x"
	} // end "for y"

}

char *char_to_bitmap(char a) {

	switch (a) {
	case ('R'):
	case ('r'):
		return R;
		break;
	case ('N'):
	case ('n'):
		return N;
		break;
	case ('A'):
		return A;
		break;
	case ('B'):
		return B;
		break;
	case ('T'):
	case ('t'):
		return T;
		break;
	case ('I'):
	case ('i'):
		return I;
		break;
	case ('M'):
		return M;
		break;
	case ('E'):
	case ('e'):
		return E;
		break;
	case ('F'):
	case ('f'):
		return F;
		break;
	case ('S'):
	case ('s'):
		return S;
		break;
	case ('L'):
	case ('l'):
		return L;
	case ('D'):
	case ('d'):
		return L;
		break;
	case ('0'):
		return zero;
		break;
	case ('1'):
		return one;
		break;
	case ('2'):
		return two;
		break;
	case ('3'):
		return three;
		break;
	case ('4'):
		return four;
		break;
	case ('5'):
		return five;
		break;
	case ('6'):
		return six;
		break;
	case ('7'):
		return seven;
		break;
	case ('8'):
		return eight;
		break;
	case ('9'):
		return nine;
		break;
	case (' '):
		return espacio;
		break;
	case (':'):
		return doublepoint;
		break;
	case ('"'):
		return comillas;
		break;
	case ('.'):
		return point;
		break;
	}
	return espacio;

}

void draw_char1(screen_t* this, char a, int textX, int textY, short textC) {

	int y = 0;
	int x = 0;

	char *character = char_to_bitmap(a);
// loop through pixel rows
	for (y = 0; y < 8; y++) {
		// loop through pixel columns
		for (x = 0; x < 8; x++) {

			// get the pixel value
			char b = character[y * 8 + x];
			if (b > 0) { // plot the pixel
				put_pixel(this, textX + (x * 2), textY + y, textC);
			} else {
				// leave empty (or maybe plot 'text backgr color')
			}
		} // end "for x"
	} // end "for y"

}

void draw_line(screen_t* this, int nlinea, int color, char c[], int size) {

	int xInit = 16;
	int i = 0;
	for (i = 0; (i < size) && c[i]; i++) {

		draw_char1(this, c[i], xInit + (16 * i), nlinea * 8, color);

	}

}

void draw_line_x2(screen_t* this, int nlinea, int color, char c[], int size) {

	int i = 0;
	int xInit = 16;
	for (i = 0; (i < size) && c[i]; i++) {

		draw_char_x2(this, c[i], xInit + (32 * i), nlinea * 16, color);

	}

}

void screen_destroy() {
	munmap(mini_screen->fb_pointer, mini_screen->finfo.smem_len);
	close(mini_screen->fbfd);
	free(mini_screen);
}

/*
  Local variables:
    mode: c
    c-file-style: stroustrup
  End:
*/
