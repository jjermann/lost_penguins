/* 
 * Copyright 2005 by Jonas Jermann
 *
 * Uses libpng (which uses zlib), so see according licenses.
 * Based on vo_png.c from Felix Buenemann <atmosfear@user.sf.net>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvlextract.h"

char* write_xpm(char* ptr, int counter){
    int size; 
    int *width, *height;
    char buf[100];
    int k,l;
    FILE *out_file;

    /* get size */
    width=(int *)(ptr+4);
    height=(int *)(ptr+8);
    size=(*width)*(*height);
    /* global data offset */
    ptr+=12;
    snprintf(buf, 100, "%08d.xpm", counter);

    /* open file */
    out_file = fopen(buf,"wb");

    /* write xpm header */
    fprintf(out_file,"/* XPM */\nstatic char * %s[] = {\n\"%d %d 256 2\",\n",buf,*width,*height);
    for (k=0; k<256; k++) {
        fprintf(out_file,"\"%s\tc #%02x%02x%02x\",\n",xpm_map[k],lvl_palette[k][0],lvl_palette[k][1],lvl_palette[k][2]);
    }

    /* reopen file for appending */
    fclose(out_file);
    out_file=fopen(buf,"a");
/*    out_file=freopen(buf,"a",out_file); */

    /* data */
    for (k=0; k<(*height); k++) {
        fprintf(out_file,"\"");
        for (l=0; l<(*width); l++) {
            fprintf(out_file,"%s",xpm_map[(unsigned char)(*(ptr+k*(*width)+l))]);
        }
        if (k==((*height)-1)) fprintf(out_file,"\"};\n");
        else fprintf(out_file,"\",\n");
    }

    fclose(out_file);

    return ptr+size;
}
