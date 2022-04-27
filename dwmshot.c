#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <png.h>

#define STATIC_ASSERT(cond) extern void dwmshot_static_assert(int hack[(cond)?1:-1])
#define STATIC_STR_LEN(str) ((sizeof((str))/sizeof((str)[0])) - 1)

#include "config.h"

static void save_png(int w, int h, char* data, int pitch)
{
	FILE* fp;
	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep* row_pointers;
	time_t now;
	char pathbuf[sizeof(save_dir) + 32];

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	assert(png_ptr != NULL);

	info_ptr = png_create_info_struct(png_ptr);
	assert(info_ptr != NULL);

	png_set_IHDR(
		png_ptr,
		info_ptr,
		w,
		h,
		8,
		PNG_COLOR_TYPE_RGB_ALPHA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE,
		PNG_FILTER_TYPE_BASE
	);

	row_pointers = malloc(sizeof(*row_pointers) * h);

	for (int i = 0; i < h; ++i)
		row_pointers[i] = data + (i * pitch);

	
	now = time(NULL);
	sprintf(pathbuf, "%s/%zu.png", save_dir, now);
	fp = fopen(pathbuf, "wb");
	assert(fp != NULL);
	png_init_io(png_ptr, fp);
	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR, NULL);
	fclose(fp);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	free(row_pointers);
}

int main(int argc, char **argv)
{
	XImage *img;
	Display *dpy;
	Window win;
	XWindowAttributes attr;

	STATIC_ASSERT(STATIC_STR_LEN(save_dir) > 0);

	dpy = XOpenDisplay(NULL);
	win = RootWindow(dpy, 0);


	XGrabServer(dpy);
	XGetWindowAttributes(dpy, win, &attr);
	img = XGetImage(dpy, win, 0, 0, attr.width, attr.height, 0xffffffff,
	                ZPixmap);
	assert(img != NULL);
			
	XUngrabServer(dpy);
	XCloseDisplay(dpy);

	save_png(
		img->width, 
		img->height,
	        img->data,
		img->width * (img->bits_per_pixel / 8)
	);

	XDestroyImage(img);
	return EXIT_SUCCESS;
}

