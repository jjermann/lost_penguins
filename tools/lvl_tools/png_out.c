/* 
 * Copyright 2005 by Jonas Jermann
 *
 * NOT WORKING!!
 * Uses libpng (which uses zlib), so see according licenses.
 * Based on vo_png.c from Felix Buenemann <atmosfear@user.sf.net>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <png.h>
#include "lvlextract.h"

int z_compression = Z_NO_COMPRESSION;

struct pngdata {
	FILE * fp;
	png_structp png_ptr;
	png_infop info_ptr;
	enum {OK,ERROR} status;  
};

/* TODO: read palette information */
struct pngdata create_png(char * fname, int image_width, int image_height) {
    struct pngdata png;
    png_color palette[256];
/*    png_colorp palette; */

    int k;

    png.png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png.info_ptr = png_create_info_struct(png.png_ptr);
   
    if (!png.png_ptr) {
       png.status = ERROR;
       return png;
    }   
    
    if (!png.info_ptr) {
       png_destroy_write_struct(&png.png_ptr, (png_infopp)NULL);
       png.status = ERROR;
       return png;
    }
    
    if (setjmp(png.png_ptr->jmpbuf)) {
        png_destroy_write_struct(&png.png_ptr, &png.info_ptr);
        fclose(png.fp);
        png.status = ERROR;
        return png;
    }
    
    png.fp = fopen (fname, "wb");
    if (png.fp == NULL) {
    	printf("\nPNG Error opening %s for writing!\n", strerror(errno));
       	png.status = ERROR;
       	return png;
    }	    
    
    png_init_io(png.png_ptr, png.fp);

    /* set the zlib compression level */
    png_set_compression_level(png.png_ptr, z_compression);
    
    png_set_IHDR(png.png_ptr, png.info_ptr, image_width, image_height,
       8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
       PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

/*    palette = (png_colorp)png_malloc(png.png_ptr, 256 * png_sizeof (png_color)); */

    for (k=0; k<256; k++) {
        palette[k].red   = lvl_palette[k][0];
        palette[k].green = lvl_palette[k][1];
        palette[k].blue  = lvl_palette[k][2];
/*
        palette[k].red   = (png_byte)lvl_palette2[k];
        palette[k].green = (png_byte)((png_bytep)lvl_palette2[k]+8);
        palette[k].blue  = (png_byte)((png_bytep)lvl_palette2[k]+16);
*/
    }

    png_set_PLTE(png.png_ptr, png.info_ptr, palette, 256);
    png_write_info(png.png_ptr, png.info_ptr);
    
    png.status = OK;
    return png;
}    

static int destroy_png(struct pngdata png) {
    png_write_end(png.png_ptr, png.info_ptr);

    png_destroy_write_struct(&png.png_ptr, &png.info_ptr);
    
    fclose (png.fp);

    return 0;
}

char* write_png(char* ptr, int counter){
    int size; 
    int *width, *height;
    char buf[100];
    int k;
    struct pngdata png;

    /* get size */
    width=(int *)(ptr+4);
    height=(int *)(ptr+8);
    size=(*width)*(*height);
    /* global data offset */
    ptr+=12;
    png_bytep row_pointers[(*height)];

    snprintf(buf, 100, "%08d.png", counter);

    png = create_png(buf, *width, *height);

    if(png.status){
	    printf("PNG Error in create_png\n");
	    exit(2);
    }	     

    for ( k = 0; k < *height; k++ )
      row_pointers[k] = (png_bytep)(ptr+k*(*width));

printf("trying to write image...\n---------\n");

    png_write_image(png.png_ptr, row_pointers);
printf("\n----------\n");
    destroy_png(png);
printf("DONE.\n");

    return ptr+size;
}

