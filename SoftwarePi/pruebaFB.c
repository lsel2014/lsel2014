#include "pruebaFB.h"
#include <sys/ioctl.h>

// 'global' variables to store screen info

char *fbp = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

// variables
int fbfd = 0;
struct fb_var_screeninfo orig_vinfo;
long int screensize = 0;
//int i = 0;

//Characters.

char A[] = { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0,
		1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

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

// helper function to 'plot' a pixel in given color
void put_pixel(int x, int y, short c) {
	// calculate the pixel's byte offset inside the buffer
	unsigned int pix_offset = x + y * finfo.line_length;

	char upper = (c >> 8) & 0xFF;
	char lower = c & 0xFF;

	// now this is about the same as 'fbp[pix_offset] = value'
	*((char*) (fbp + pix_offset)) = upper;
	*((char*) (fbp + pix_offset + 1)) = lower;

}

// helper function for drawing - no more need to go mess with
// the main function when just want to change what to draw...

void draw(short c) {

	int x, y;

	for (y = 0; y < vinfo.yres - 1; y++) {
		for (x = 0; x < vinfo.xres * 2; x++) {

			// color based on the 16th of the screen width
			// short c = 0x1818;

			// call the helper function

			put_pixel(x, y, c);

		}
	}

}

void draw_char(char *a, int textX, int textY, short textC) {
	int y = 0;
	int x = 0;
	// loop through pixel rows
	for (y = 0; y < 8; y++) {
		// loop through pixel columns
		for (x = 0; x < 8; x++) {
			// get the pixel value
			char b = a[y * 8 + x];
			if (b > 0) { // plot the pixel
				put_pixel(textX + (x * 2), textY + y, textC);
			} else {
				// leave empty (or maybe plot 'text backgr color')
			}
		} // end "for x"
	} // end "for y"

}

// Function to draw a char bigger.
void draw_char_x2(char a, int textX, int textY, int textC) {
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
				put_pixel(textX + (x * 4), textY + (y * 2), textC);
				put_pixel(textX + (x * 4) + 2, textY + (y * 2), textC);
				put_pixel(textX + (x * 4), textY + (y * 2) + 1, textC);
				put_pixel(textX + (x * 4) + 2, textY + (y * 2) + 1, textC);
			} else {
				// leave empty (or maybe plot 'text backgr color')
			}
		} // end "for x"
	} // end "for y"

}

char *char_to_bitmap(char a) {

	switch (a) {
	case ('A'):
		return A;
		break;
	case ('B'):
		return B;
		break;
	case ('T'):
		return T;
		break;
	case ('I'):
		return I;
		break;
	case ('M'):
		return M;
		break;
	case ('E'):
		return E;
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

void draw_char1(char a, int textX, int textY, short textC) {

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
				put_pixel(textX + (x * 2), textY + y, textC);
			} else {
				// leave empty (or maybe plot 'text backgr color')
			}
		} // end "for x"
	} // end "for y"

}

void draw_line(int nlinea, int color, char c[], int size) {

	int xInit = 16;
	int i = 0;
	for (i = 0; i < size && c[i] != '\0'; i++) {
		
		draw_char1(c[i], xInit + (16 * i), nlinea * 8, color);

	}

}

void draw_line_x2(int nlinea, int color, char c[], int size) {

	int i = 0;
	int xInit = 16;
	for (i = 0; i < size && c[i] != '\0'; i++) {

		draw_char_x2(c[i], xInit + (32 * i), nlinea * 16, color);

	}

}

void fbtft_init() {
	// Open the file for reading and writing
	fbfd = open("/dev/fb1", O_RDWR);
	if (!fbfd) {
		printf("Error: cannot open framebuffer device.\n");
	}
	printf("The framebuffer device was opened successfully.\n");

	// Get variable screen information
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		printf("Error reading variable information.\n");
	}
	printf("Original %dx%d, %dbpp\n", vinfo.xres, vinfo.yres,
			vinfo.bits_per_pixel);

	// Store for reset (copy vinfo to vinfo_orig)
	memcpy(&orig_vinfo, &vinfo, sizeof(struct fb_var_screeninfo));

	// Change variable info
	vinfo.bits_per_pixel = 16;
	if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
		printf("Error setting variable information.\n");
	}

	// Get fixed screen information
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
		printf("Error reading fixed information.\n");
	}
	// map fb to user mem
	screensize = finfo.smem_len;
	fbp = (char*) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd,
			0);

	if ((int) fbp == -1) {
		printf("Failed to mmap.\n");
	}
}

void fbtft_destroy() {
	munmap(fbp, screensize);
	close(fbfd);
}

// application entry point
//int main(int argc, char* argv[])
//{
//    // Open the file for reading and writing
//    fbfd = open("/dev/fb1", O_RDWR);
//    if (!fbfd) {
//      printf("Error: cannot open framebuffer device.\n");
//      return(1);
//    }
//    printf("The framebuffer device was opened successfully.\n");
//
//    // Get variable screen information
//    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
//      printf("Error reading variable information.\n");
//    }
//    printf("Original %dx%d, %dbpp\n", vinfo.xres, vinfo.yres,
//       vinfo.bits_per_pixel );
//
//    // Store for reset (copy vinfo to vinfo_orig)
//    memcpy(&orig_vinfo, &vinfo, sizeof(struct fb_var_screeninfo));
//
//    // Change variable info
//    vinfo.bits_per_pixel = 16;
//    if (ioctl(fbfd, FBIOPUT_VSCREENINFO, &vinfo)) {
//      printf("Error setting variable information.\n");
//    }
//
//    // Get fixed screen information
//    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
//      printf("Error reading fixed information.\n");
//    }
//    // map fb to user mem
//    screensize = finfo.smem_len;
//    fbp = (char*)mmap(0,
//              screensize,
//              PROT_READ | PROT_WRITE,
//              MAP_SHARED,
//              fbfd,
//              0);
//
//
//    if ((int)fbp == -1) {
//        printf("Failed to mmap.\n");
//    }
//    else {
//
//    	// and lower half with something else
//
//	// draw...
//	int i=0;
//
//        draw(0x1818);
//        //sleep(1);
//	int size = 32;
//	char c[] = "BABABA";
//
//	//draw_char1('A',200, 200, 0xffff);
//	draw_line(2, 0xffff, c, 6);
//	//draw_line_x2(2, 0xffff, "BABA");
//	//draw_line_x2(3, 0xab12, "ABAAAA");
//	//draw_line_x2(4, 0xffff, "BAAAAAAA");
//	//draw_line_x2(5, 0xab12, "BBBBBBAAA");
//
//	//for(i=0; i<50; i++){
//		//draw_char1(A,200 + (size *i),200,3+(i*9));
//		//draw_char_x2(A,200 + (size *i),200,3+(i*9));
//	//}
//	//for(i=0; i<50; i++){
//      	//  draw_char_x2(A,200 + (size *i),216,0xffff);
//        //}
//	sleep(1);
//
//
//    }
//
//    // cleanup
//    munmap(fbp, screensize);
//
//    close(fbfd);
//
//    return 0;
//
//}
