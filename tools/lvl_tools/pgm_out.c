/* 
 * Copyright 2005 by Jonas Jermann
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "lvlextract.h"

char* write_pgm(char* ptr, int counter) {
    int size;
    FILE *out_file;
    char pbm_header[50];
    char filename[50];
    int *width, *height;

    /* get size */
    width=(int *)(ptr+4);
    height=(int *)(ptr+8);
    size=(*width)*(*height);
    /* global data offset */
    ptr+=12;
    /* pbm header */
    sprintf(pbm_header,"P5\n%d %d\n255\n",*width,*height);
    sprintf(filename,"test_%010d.pgm",counter);

    out_file = fopen(filename,"wb");
    fprintf(out_file,"P5\n%d %d\n255\n",*width,*height);
    fclose(out_file);
    out_file = fopen(filename,"a");
    fwrite(ptr,1,size,out_file);
    fclose(out_file);

    return ptr+size;
}
