/*
 * (C) 2004 Jonas Jermann
 *
 * Extract Images from .lvl files using ImageMagick api
 *
 * License: GPL
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>  
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <magick/api.h>

#define EXIT(a) { errno=(a); goto end; }

enum {
    red=0,
    green=1,
    blue=2
};

enum {
    errno_ok=0,
    errno_quit=1,
    errno_parse=2,
    errno_open=3,
    errno_invalid=4,
    errno_alloc=5,
    errno_misc=6
};

typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int w;
    unsigned int h;
} Rectangle;

typedef struct {
    unsigned int start_num;
    unsigned int size;
    char name[50];
    Rectangle* geometry;
} LVLAnim;

typedef struct {
    /* create unknown.bin chunk files if != 0 */
    int debug;
    /* start index (for the frame names) */
    unsigned int start_index;
    /* Write the individial animations */
    int write;
    /* input lvl file */
    char lvl_file[20];
    /* input data file */
    char data_file[20];
    /* output geometry file */
    char geom_file[20];
    /* base name (used for output directory and output file names) */
    char basename[20];
    /* image format */
    char format[5];
    /* colorkey */
    PixelPacket colorkey;
    /* background color */
    PixelPacket bg;
} Config;

Config config;
int errno=errno_ok;

/* RGB palette */
unsigned char lvl_palette[256][3] = {
    /* The first two RGB values represent the colorkey (changed accordingly later) */
    { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00, },

                                                { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 },
    { 0x08, 0x04, 0x00 }, { 0x00, 0x04, 0x10 }, { 0x08, 0x10, 0x00 }, { 0x08, 0x10, 0x10 },
    { 0x18, 0x10, 0x00 }, { 0x18, 0x14, 0x10 }, { 0x00, 0x08, 0x29 }, { 0x00, 0x0c, 0x39 },
    { 0x10, 0x14, 0x20 }, { 0x18, 0x18, 0x20 }, { 0x08, 0x20, 0x08 }, { 0x08, 0x24, 0x29 },
    { 0x18, 0x28, 0x39 }, { 0x29, 0x10, 0x08 }, { 0x20, 0x24, 0x00 }, { 0x20, 0x24, 0x10 },
    { 0x31, 0x24, 0x00 }, { 0x31, 0x24, 0x00 }, { 0x31, 0x34, 0x00 }, { 0x31, 0x34, 0x10 },
    { 0x20, 0x20, 0x20 }, { 0x29, 0x24, 0x20 }, { 0x20, 0x34, 0x29 }, { 0x31, 0x30, 0x29 },
    { 0x31, 0x30, 0x39 }, { 0x00, 0x0c, 0x4a }, { 0x62, 0x65, 0x08 }, { 0x73, 0x6d, 0x52 },
    { 0x73, 0x00, 0xb4 }, { 0x94, 0x38, 0x20 }, { 0xa4, 0x65, 0x39 }, { 0x31, 0x34, 0x4a },
    { 0x29, 0x34, 0x62 }, { 0x08, 0x44, 0x10 }, { 0x00, 0x65, 0x00 }, { 0x29, 0x59, 0x39 },
    { 0x31, 0x7d, 0x39 }, { 0x29, 0x48, 0x6a }, { 0x4a, 0x0c, 0x08 }, { 0x41, 0x2c, 0x00 },
    { 0x41, 0x28, 0x10 }, { 0x52, 0x2c, 0x10 }, { 0x52, 0x3c, 0x10 }, { 0x4a, 0x38, 0x29 },
    { 0x41, 0x3c, 0x39 }, { 0x62, 0x1c, 0x08 }, { 0x73, 0x08, 0x08 }, { 0x62, 0x3c, 0x10 },
    { 0x7b, 0x20, 0x08 }, { 0x73, 0x34, 0x10 }, { 0x41, 0x3c, 0x4a }, { 0x41, 0x40, 0x00 },
    { 0x41, 0x40, 0x10 }, { 0x4a, 0x55, 0x00 }, { 0x52, 0x40, 0x00 }, { 0x52, 0x55, 0x08 },
    { 0x52, 0x55, 0x18 }, { 0x52, 0x44, 0x39 }, { 0x5a, 0x59, 0x39 }, { 0x62, 0x44, 0x00 },
    { 0x62, 0x55, 0x00 }, { 0x62, 0x55, 0x18 }, { 0x73, 0x44, 0x10 }, { 0x73, 0x59, 0x00 },
    { 0x73, 0x59, 0x10 }, { 0x62, 0x40, 0x20 }, { 0x62, 0x44, 0x39 }, { 0x73, 0x40, 0x20 },
    { 0x7b, 0x44, 0x20 }, { 0x7b, 0x55, 0x20 }, { 0x7b, 0x59, 0x31 }, { 0x62, 0x65, 0x08 },
    { 0x62, 0x65, 0x18 }, { 0x62, 0x75, 0x18 }, { 0x7b, 0x6d, 0x08 }, { 0x73, 0x6d, 0x18 },
    { 0x7b, 0x7d, 0x08 }, { 0x7b, 0x7d, 0x18 }, { 0x7b, 0x6d, 0x29 }, { 0x7b, 0x7d, 0x29 },
    { 0x4a, 0x48, 0x52 }, { 0x52, 0x50, 0x52 }, { 0x4a, 0x4c, 0x6a }, { 0x4a, 0x4c, 0x7b },
    { 0x5a, 0x59, 0x62 }, { 0x4a, 0x71, 0x5a }, { 0x6a, 0x55, 0x52 }, { 0x73, 0x6d, 0x52 },
    { 0x62, 0x65, 0x62 }, { 0x62, 0x6d, 0x7b }, { 0x73, 0x71, 0x6a }, { 0x7b, 0x79, 0x7b },
    { 0x08, 0x10, 0x94 }, { 0x00, 0x04, 0xac }, { 0x10, 0x1c, 0xa4 }, { 0x18, 0x20, 0xbd },
    { 0x20, 0x30, 0x83 }, { 0x00, 0x04, 0xf6 }, { 0x29, 0x30, 0xc5 }, { 0x29, 0x4c, 0x83 },
    { 0x20, 0x40, 0xa4 }, { 0x20, 0x55, 0xc5 }, { 0x20, 0x4c, 0xee }, { 0x73, 0x00, 0xb4 },
    { 0x4a, 0x4c, 0xac }, { 0x4a, 0x71, 0x83 }, { 0x52, 0x6d, 0xac }, { 0x4a, 0x48, 0xcd },
    { 0x52, 0x4c, 0xee }, { 0x5a, 0x6d, 0xe6 }, { 0x00, 0x99, 0x00 }, { 0x10, 0xea, 0x08 },
    { 0x4a, 0x85, 0x39 }, { 0x4a, 0xa5, 0xac }, { 0x7b, 0x85, 0xa4 }, { 0x5a, 0xba, 0xee },
    /* check below */
    { 0x6a, 0x89, 0xee }, { 0x94, 0x08, 0x08 }, { 0x94, 0x2c, 0x08 }, { 0x94, 0x38, 0x20 },
    { 0xac, 0x0c, 0x08 }, { 0xb4, 0x30, 0x10 }, { 0xb4, 0x34, 0x31 }, { 0x94, 0x50, 0x10 },
    { 0x8b, 0x4c, 0x20 }, { 0x8b, 0x5d, 0x20 }, { 0x8b, 0x5d, 0x31 }, { 0x8b, 0x69, 0x00 },
    { 0x9c, 0x69, 0x08 }, { 0x94, 0x7d, 0x00 }, { 0x94, 0x79, 0x39 }, { 0xac, 0x55, 0x00 },
    { 0xa4, 0x59, 0x20 }, { 0xa4, 0x5d, 0x31 }, { 0xbd, 0x50, 0x20 }, { 0xa4, 0x65, 0x39 },
    { 0xa4, 0x7d, 0x20 }, { 0xac, 0x75, 0x39 }, { 0xbd, 0x65, 0x29 }, { 0xbd, 0x6d, 0x39 },
    { 0x8b, 0x61, 0x4a }, { 0x9c, 0x79, 0x5a }, { 0xd5, 0x08, 0x08 }, { 0xd5, 0x30, 0x20 },
    { 0xf6, 0x08, 0x08 }, { 0xf6, 0x28, 0x20 }, { 0xf6, 0x38, 0x39 }, { 0xc5, 0x59, 0x39 },
    { 0xc5, 0x79, 0x29 }, { 0xc5, 0x7d, 0x39 }, { 0xde, 0x7d, 0x31 }, { 0xe6, 0x59, 0x29 },
    { 0xe6, 0x59, 0x52 }, { 0xff, 0x59, 0x52 }, { 0xb4, 0x24, 0xbd }, { 0x8b, 0x81, 0x10 },
    { 0x94, 0x95, 0x00 }, { 0x8b, 0x81, 0x20 }, { 0x9c, 0x99, 0x20 }, { 0xa4, 0x81, 0x00 },
    { 0xa4, 0x81, 0x10 }, { 0xac, 0x9d, 0x00 }, { 0xbd, 0x81, 0x08 }, { 0xac, 0x99, 0x20 },
    { 0xb4, 0xae, 0x18 }, { 0xa4, 0xaa, 0x20 }, { 0xb4, 0xae, 0x29 }, { 0xbd, 0xb6, 0x39 },
    { 0x94, 0x81, 0x73 }, { 0xa4, 0x99, 0x41 }, { 0xbd, 0x99, 0x5a }, { 0xb4, 0x8d, 0x73 },
    { 0xbd, 0xaa, 0x73 }, { 0xc5, 0x9d, 0x00 }, { 0xcd, 0x99, 0x20 }, { 0xcd, 0xb6, 0x00 },
    { 0xc5, 0xb6, 0x29 }, { 0xd5, 0xba, 0x20 }, { 0xde, 0xba, 0x39 }, { 0xff, 0x99, 0x00 },
    { 0xe6, 0xae, 0x00 }, { 0xc5, 0x81, 0x4a }, { 0xc5, 0x95, 0x41 }, { 0xd5, 0x99, 0x41 },
    { 0xd5, 0x99, 0x52 }, { 0xde, 0x85, 0x73 }, { 0xd5, 0xaa, 0x5a }, { 0xd5, 0xb2, 0x6a },
    { 0xd5, 0xb6, 0x7b }, { 0xe6, 0x81, 0x52 }, { 0xf6, 0x85, 0x73 }, { 0xee, 0xa5, 0x5a },
    { 0xf6, 0xb2, 0x73 }, { 0xd5, 0xce, 0x00 }, { 0xd5, 0xca, 0x31 }, { 0xd5, 0xd2, 0x20 },
    { 0xde, 0xda, 0x39 }, { 0xf6, 0xd2, 0x00 }, { 0xff, 0xf2, 0x00 }, { 0xee, 0xee, 0x20 },
    { 0xf6, 0xf2, 0x39 }, { 0xd5, 0xca, 0x41 }, { 0xde, 0xda, 0x4a }, { 0xee, 0xde, 0x52 },
    { 0xf6, 0xde, 0x73 }, { 0xee, 0xe2, 0x41 }, { 0xf6, 0xf2, 0x52 }, { 0x8b, 0x89, 0x83 },
    { 0x94, 0x95, 0x9c }, { 0x8b, 0x99, 0xb4 }, { 0xb4, 0xa5, 0x94 }, { 0xa4, 0xa5, 0xac },
    { 0xb4, 0xb2, 0xb4 }, { 0xbe, 0xbd, 0xbe }, { 0x94, 0x91, 0xf6 }, { 0xac, 0xb6, 0xd5 },
    { 0xac, 0xb2, 0xff }, { 0xbd, 0xd2, 0xee }, { 0xd5, 0xb6, 0x94 }, { 0xcd, 0xba, 0xb4 },
    { 0xf6, 0x99, 0x94 }, { 0xf6, 0xba, 0x9c }, { 0xde, 0xca, 0x8b }, { 0xde, 0xce, 0xa4 },
    { 0xde, 0xd2, 0xb4 }, { 0xff, 0xde, 0x94 }, { 0xf6, 0xda, 0xbd }, { 0xff, 0xfa, 0xbd },
    { 0xcd, 0xca, 0xcd }, { 0xde, 0xda, 0xcd }, { 0xde, 0xda, 0xde }, { 0xd5, 0xf6, 0xff },
    { 0xe6, 0xe2, 0xee }, { 0xf6, 0xf2, 0xe6 }, { 0xff, 0xfa, 0xff }, { 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }, { 0x00, 0x00, 0x00 }
};


void usage() {
    printf("Usage: lvl2magick [options] file.lvl\n");
    printf("Extracts images from file.lvl.\n\n");
    printf("Options:\n");
    printf("  -h, --help             Print this help message\n\n");
    printf("  -debug                 Create additional debug files (default: off)\n\n");
    printf("  -start_index <n>       Start index of the frame image names (default: 0)\n\n");
    printf("  -write <0-7>           n=0: Just create the geometry data (no images)\n");
    printf("                         n=1: Create one big image with all animations in it\n");
    printf("                         n=2: Create one image for each animation\n");
    printf("                         n=4: Create one image for each frame\n");
    printf("                         Values can be added to achieve the desired combination\n");
    printf("                           (default: 3)\n\n");
    printf("  -colorkey \"#rrggbb\"    Use the specified color for the colorkey. The numbers\n");
    printf("                         rr, gg, bb are hex values from 0 to ff. (default: #ff00ff)\n\n");
    printf("  -bg \"#rrggbb\"          Use the specified color for the background (default: colorkey)\n\n");
    printf("  -dir, -o <basename>    Name of the output directory (default: file)\n\n");
    printf("  -format <format>       Name of the output image format (default: png)\n\n");
    printf("  -data_file <filename>  Data filename for file.lvl (default: file.dat)\n\n");
}

int parse(int argc, char* argv[]) {
    int i;
    char buf[20];

    /* default settings */
    strncpy(config.lvl_file,"",16);
    strncpy(config.data_file,"",16);
    strncpy(config.basename,"",16);
    strncpy(config.format,"png",4);
    config.debug=0;
    config.start_index=0;
    config.write=3;
    config.colorkey.red=MaxRGB;
    config.colorkey.green=0;
    config.colorkey.blue=MaxRGB;
    config.colorkey.opacity=0;
    config.bg.red=config.colorkey.red;
    config.bg.green=config.colorkey.green;
    config.bg.blue=config.colorkey.blue;
    config.bg.opacity=config.colorkey.opacity;

    /* parse cmdline */
    for (i=1; i<argc; i++) {
        if (strncmp(argv[i],"-",1)==0) {
            if (strcmp(argv[i],"--help")==0 || strcmp(argv[i],"-h")==0) {
                usage();
                return errno_quit;
            } else if (strcmp(argv[i],"-debug")==0) {
                config.debug=1;
            } else if (strcmp(argv[i],"-start_index")==0) {
                i++;
                if (strncmp(argv[i],"-",1)==0) { usage(); return errno_parse; }
                config.start_index=(unsigned int)atoi(argv[i]);
            } else if (strcmp(argv[i],"-write")==0) {
                i++;
                if (strncmp(argv[i],"-",1)==0) { usage(); return errno_parse; }
                config.write=atoi(argv[i]);
                if (config.write>7 || config.write<0) {
                    usage();
                    return errno_parse;
                }
            } else if (strcmp(argv[i],"-colorkey")==0) {
                i++;
                if (!strncmp(argv[i],"#",1)==0) { usage(); return errno_parse; }
                strcpy(buf,argv[i]+1); buf[2]=0;
                config.colorkey.red=(Quantum)(MaxRGB*strtol(buf,NULL,16)/255);
                strcpy(buf,argv[i]+3); buf[2]=0;
                config.colorkey.green=(Quantum)(MaxRGB*strtol(buf,NULL,16)/255);
                strcpy(buf,argv[i]+5); buf[2]=0;
                config.colorkey.blue=(Quantum)(MaxRGB*strtol(buf,NULL,16)/255);
                config.bg.blue=config.colorkey.blue;
                config.bg.red=config.colorkey.red;
                config.bg.green=config.colorkey.green;
            } else if (strcmp(argv[i],"-bg")==0) {
                i++;
                if (strncmp(argv[i],"#",1)==0) {
                    strcpy(buf,argv[i]+1); buf[2]=0;
                    config.bg.red=(Quantum)(MaxRGB*strtol(buf,NULL,16)/255);
                    strcpy(buf,argv[i]+3); buf[2]=0;
                    config.bg.green=(Quantum)(MaxRGB*strtol(buf,NULL,16)/255);
                    strcpy(buf,argv[i]+5); buf[2]=0;
                    config.bg.blue=(Quantum)(MaxRGB*strtol(buf,NULL,16)/255);
                    config.bg.opacity=0;
                } else {
                    usage();
                    return errno_parse;
                }
            } else if (strcmp(argv[i],"-dir")==0 || strcmp(argv[i],"-o")==0) {
                i++;
                if (strncmp(argv[i],"-",1)==0) { usage(); return errno_parse; }
                strncpy(config.basename,argv[i],16);
            } else if (strcmp(argv[i],"-format")==0) {
                i++;
                if (strncmp(argv[i],"-",1)==0) { usage(); return errno_parse; }
                strncpy(config.format,argv[i],4);
            } else if (strcmp(argv[i],"-data_file")==0) {
                i++;
                if (strncmp(argv[i],"-",1)==0) { usage(); return errno_parse; }
                strncpy(config.data_file,argv[i],16);
            } else {
                printf("Unknown suboption %s!\n",argv[i]);
            }
        } else if (strcmp(config.lvl_file,"")==0) {
            strncpy(config.lvl_file,argv[i],16);
        } else {
            printf("Option %s ignored, filename is already set!\n",argv[i]);
        }
    }

    /* check validity */
    if (strcmp(config.lvl_file,"")==0) {
        usage();
        return errno_parse;
    }

    /* default settings if unspecified */
    if (strcmp(config.basename,"")==0) {
        /* copy the filename */
        if (strrchr(config.lvl_file, '/')) strncpy(config.basename, strrchr(config.lvl_file, '/')+1, 16);
        else strncpy(config.basename, config.lvl_file, 16);

        /* truncate filename at . */
        if (strrchr(config.basename, '.')) {   
            char *loc = strrchr(config.basename, '.');
            *loc = 0;
        }
    }
    if (strcmp(config.data_file,"")==0) {
        snprintf(config.data_file,16,"%s.dat",config.basename);
    }
    snprintf(config.geom_file,16,"%s.txt",config.basename);

    /* set colorkey in palette */
    lvl_palette[0][red]   = (unsigned char)(config.colorkey.red*255/MaxRGB);
    lvl_palette[0][green] = (unsigned char)(config.colorkey.green*255/MaxRGB);
    lvl_palette[0][blue]  = (unsigned char)(config.colorkey.blue*255/MaxRGB);
    lvl_palette[1][red]   = (unsigned char)(config.colorkey.red*255/MaxRGB);
    lvl_palette[1][green] = (unsigned char)(config.colorkey.green*255/MaxRGB);
    lvl_palette[1][blue]  = (unsigned char)(config.colorkey.blue*255/MaxRGB);

    return errno_ok;
}

void freeLVLAnimList(LVLAnim* lvlanims,unsigned int size) {
    unsigned int i;
    for (i=0; i< size; i++) {
        free(lvlanims[i].geometry);
    }
    free(lvlanims);
}

/* Parses the specified data file and stores the result (array of LVLAnim) in
 * lvlanims. The return value is the size of the array (number of animations).
 * Usage: anim_size=parseDataFile("image.dat",&lvlanims);
 *
 * Data file Format:
 *
 * -----------------
 * LVLDATA
 *
 * start_offset size name
 * ...
 *
 * -----------------
 *
 * start_offset is the image index off all images in the file
 * size is the number of frames of the animation
 * name is the unique name of the animation
 *
 */
unsigned int parseDataFile(char* data_file_name, LVLAnim** lvlanims) {
    const char data_id[]="LVLDATA";
    unsigned int start_num, size, data_size=0;
    char name[50], line[80];
    int match=0;
    unsigned int i=0;

    FILE* data_file = fopen(data_file_name,"r");
    *lvlanims=NULL;

    if (data_file == NULL) {
        printf("Data file %s not found!\n",data_file_name);
        return 0;
    }

    if (!fgets(line,80,data_file) || strncmp(line,data_id,7)) {
        printf("Data file %s is invalid!!\n",data_file_name);
        fclose(data_file);
        return 0;
    }

    /* get the number of entries */
    while (fgets(line,80,data_file)) {
        match=sscanf(line, "%u %u %s\n", &start_num, &size, name);
        if (match==3) data_size++;
    }
    if (data_size==0) {
        printf("Data file %s is invalid (no entries)!\n",data_file_name);
        fclose(data_file);
        return 0;
    }
    /* rewind to the beginning */
    rewind(data_file);
    (void)fgets(line,80,data_file);

    /* allocate memory */
    if ((*lvlanims=(LVLAnim*)malloc(data_size*sizeof(LVLAnim))) == NULL) {
        printf("Memory allocation of LVLAnim* failed! Couldn't read data file %s!\n", data_file_name);
        *lvlanims=NULL;
        return 0;
    }

    /* parse the file */
    i=0;
    while (fgets(line,80,data_file)) {
        /* TODO: ignore lines starting with: # */
        match=sscanf(line, "%u %u %s\n", &start_num, &size, name);
        if (match!=3) continue;
        (*lvlanims)[i].start_num=start_num;
        (*lvlanims)[i].size=size;
        strncpy((*lvlanims)[i].name,name,50);

        (*lvlanims)[i].geometry=NULL;
        i++;
    }

    fclose(data_file);
    return data_size;
}


/*
 * Create an ImageMagick image of size width x height
 * 
 * image_info is an initialized ImageMagick ImageInfo* ptr
 * ptr is the pointer to the image data offset
 */
Image* getFrame(unsigned char* ptr, ImageInfo* image_info, unsigned int width, unsigned int height) {
    Image* image;
    unsigned long int y;
    register unsigned long int x;
    register PixelPacket *q;
    register unsigned char *p;

    image=AllocateImage(image_info);
    image->columns=width;
    image->rows=height;  
    image->matte_color=config.colorkey;

    p=ptr;
    for (y=0; y < image->rows; y++) {
        q=SetImagePixels(image,0,(long int)y,image->columns,1);
        if (q == (PixelPacket *) NULL) break;
        for (x=0; x < image->columns; x++) { 
            q->red=(Quantum)(MaxRGB*(lvl_palette[*p][red])/255);
            q->green=(Quantum)(MaxRGB*(lvl_palette[*p][green])/255);
            q->blue=(Quantum)(MaxRGB*(lvl_palette[*p][blue])/255); 
            p++;
            q++;
        }
        if (!SyncImagePixels(image)) break;
    }
    return image;
}

int main(int argc, char *argv[]) {
    /* ImageMagick stuff */
    Image *image_list=NULL, *anim_list=NULL;
    ImageInfo *image_info=NULL;
    MontageInfo montage_info, montage_anim_info;
    ExceptionInfo exception;
    Image *big_image=NULL, *anim_image=NULL, *tmp_image=NULL;

    /* temporary variables */
    char buf[20];
    struct stat sb;
    unsigned int i,j,width,height,x_off=0,y_off=0,maxh=0;
    int d_exception=0;

    const char entry_id[]="TRPS";
    unsigned int data_size=0, tot_entries=0, num_entries=0, unknown=0;
    FILE *lvl_file=NULL, *unknown_file=NULL, *geom_file=NULL;
    LVLAnim* lvlanims=NULL;
    unsigned int lvlanim_size=0;

    /* Initial start pointer to the beginning of the file */
    unsigned char *data=NULL;
    /* Temporary variable to indicate the last offset to start searching from:
     * Either after (1+) an invalid "T" or after the image of a valid "TRPS" */
    unsigned char *last_ptr;
    /* Either index position (start) or the last known position after an image */
    unsigned char *save_ptr;
    /* Iterates through all offsets at a "T" */
    unsigned char *off_ptr;

    const unsigned int img_off_size_step=32;
    unsigned int img_off_size=0;
    unsigned char **img_offsets=NULL;
    unsigned char **tmp=NULL;

    /* -------------------------------------------------------------------- */

    /* parse command line options and set config */
    errno=parse(argc,argv);

    if (errno==errno_quit)
        EXIT(errno_ok)
    else if (errno==errno_parse)
        EXIT(errno_parse)
    else if (errno!=errno_ok)
        EXIT(errno)

    /* Check lvl_file */
    lvl_file = fopen(config.lvl_file,"r");

    if (lvl_file == NULL) {
        printf("Error opening file\n");
        EXIT(errno_open)
    }

    (void)fread(buf,1,12,lvl_file);

    if ((buf[0] != 'D') && (buf[1] != 'A') &&
      (buf[2] != 'T') && (buf[3] != 'A')) {
        printf("Invalid file\n");
        fclose(lvl_file);
        lvl_file=NULL;
        EXIT(errno_invalid)
    }

    /* get file size */
    fstat(fileno(lvl_file), &sb);
    data_size = (unsigned int)(sb.st_size);

    /* Map the entire file into process memory space */
    if ((data = mmap(NULL, data_size, PROT_READ, MAP_PRIVATE, fileno(lvl_file), 0)) == MAP_FAILED) {
        printf("ERROR: Unable to mmap the file content!\n");
        EXIT(errno_alloc)
    }

    /* Iterate through all image data offsets to get the number of entries and the
     * size of the unknown chunk */
    tot_entries=0;
    last_ptr=data;
    save_ptr=data;
    while (!((off_ptr=(unsigned char*)memchr(last_ptr,'T',(data_size-(last_ptr-data)))) == NULL)) {
        unknown+=(off_ptr-last_ptr);
        if (!strncmp((char*)off_ptr,entry_id,4)) {
            /* Create an image offset table */
            if (img_off_size<=tot_entries) {
                if ((tmp = realloc(img_offsets, sizeof(unsigned char*) * (img_off_size += img_off_size_step))) == NULL) {
                    printf("ERROR: Realloc failed (img_offsets)!\n");
                    free(img_offsets);
                    EXIT(errno_alloc)
                }
                img_offsets = tmp;
            }
            img_offsets[tot_entries]=off_ptr;
            tot_entries++;
            /*
             * Write unknown content (up to this image position (off_ptr)) to a file.
             * If the file size would be 0, skip it.
             */
            if (config.debug && (off_ptr-save_ptr > 0)) {
                snprintf(buf, 16, "%08u.bin", unknown);
                unknown_file = fopen(buf, "wb");
                (void)fwrite(save_ptr,1,(size_t)(off_ptr-save_ptr),unknown_file);
                fclose(unknown_file);
            }
            /* skip the image */
            width=*((unsigned int *)(off_ptr+4));
            height=*((unsigned int *)(off_ptr+8));
            last_ptr=off_ptr+12+width*height;
            save_ptr=last_ptr;
        } else {
            last_ptr=off_ptr+1;
        }
    }

    /* Check data_file */
    lvlanim_size=parseDataFile(config.data_file, &lvlanims);

    if (lvlanims==NULL) {
        if ( (lvlanims=(LVLAnim*)malloc(sizeof(LVLAnim))) == NULL) {
            printf("Memory allocation of LVLAnim* failed!\n");
            lvlanims=NULL;
            EXIT(errno_alloc)
        } else {
            lvlanims->start_num=0;
            lvlanims->size=tot_entries;
            strncpy(lvlanims->name,config.basename,50);
            lvlanims->geometry=NULL;
        }
        lvlanim_size=1;
    }
    if (lvlanim_size<=1) config.write&=~2;

    /* Change to the base directory */
    mkdir(config.basename, 0777);
    chdir(config.basename);
    printf("Directory: %s\n", config.basename);

    /* ImageMagick: global stuff */
    if (config.write!=0) {
        image_info=CloneImageInfo((ImageInfo *) NULL);
        image_info->colorspace = RGBColorspace;
        image_list=NewImageList();
        GetExceptionInfo(&exception);
        d_exception=1;
        GetMontageInfo(image_info,&montage_info);
        montage_info.tile="1x3000";
        montage_info.geometry="+0+0";
        montage_info.background_color=config.bg;
        montage_info.matte_color=config.colorkey;
        montage_info.gravity=NorthWestGravity;
        GetMontageInfo(image_info,&montage_anim_info);
        montage_anim_info.tile="3000x1";
        montage_anim_info.geometry="+0+0";
        montage_anim_info.background_color=config.bg;
        montage_anim_info.matte_color=config.colorkey;
        montage_anim_info.gravity=NorthWestGravity;
    }
    y_off=0;

    /* Main loop: Process each animation */
    for (i=0; i<lvlanim_size; i++) {
        if ( (lvlanims[i].geometry=(Rectangle*)malloc(lvlanims[i].size*sizeof(Rectangle))) == NULL ) {
            printf("Memory allocation of geometry entry %u failed!\n",i);
            EXIT(errno_alloc)
        }
        x_off=0;
        maxh=0;

        /* Process each frame of the current animation */
        for (j=lvlanims[i].start_num; j<(lvlanims[i].start_num+lvlanims[i].size); j++) {
            if (j>=tot_entries) {
                printf("Illegal animation data file (image number %u does not exist)!\n",j);
                break;
            }
            /* --== Parse header ==-- */
            width=*((unsigned int *)(img_offsets[j]+4));
            height=*((unsigned int *)(img_offsets[j]+8));
            if (height>maxh) maxh=height;

            lvlanims[i].geometry[j-lvlanims[i].start_num].x=x_off;
            lvlanims[i].geometry[j-lvlanims[i].start_num].y=y_off;
            lvlanims[i].geometry[j-lvlanims[i].start_num].w=width;
            lvlanims[i].geometry[j-lvlanims[i].start_num].h=height;

            if (width*height>0) num_entries++;

            /* get the frame and append it to the animation image list */
            if (config.write!=0) {
                tmp_image=getFrame(img_offsets[j]+12,image_info,width,height);
                if (config.write&4) {
                    snprintf(buf, 80, "%s_%04u.%s",lvlanims[i].name,(j-lvlanims[i].start_num)+config.start_index,config.format);
                    strcpy(tmp_image->filename,buf);
                    (void)WriteImage(image_info,tmp_image);
                }
                AppendImageToList(&anim_list,tmp_image);
            }

            x_off+=width;
        }

        /* Montage the image and append it to the big image list */
        if (config.write!=0) {
            if (anim_list!=(Image *) ((void *)0)) {
                anim_image=MontageImages(anim_list,&montage_anim_info,&exception);
                anim_image->matte_color=config.colorkey;

                DestroyImageList(anim_list);
                anim_list=NULL;
                if (config.write&2) {
                    snprintf(buf, 80, "%s.%s",lvlanims[i].name,config.format);
                    strcpy(anim_image->filename,buf);
                    (void)WriteImage(image_info,anim_image);
                }
                AppendImageToList(&image_list,anim_image);
            } else {
                printf("Empty animation: %s!\n",lvlanims[i].name);
            }
        }
        y_off+=maxh;
    }

    /* Create a big image (montage) using the big image list */
    if (config.write!=0) {
        big_image=MontageImages(image_list,&montage_info,&exception);
        big_image->matte_color=config.colorkey;
        DestroyImageList(image_list);
        image_list=NULL;

        if (config.write&1) {
            snprintf(buf, 16, "%s.%s",config.basename,config.format);
            strcpy(big_image->filename,buf);
            (void)WriteImage(image_info,big_image);
        }
        DestroyImage(big_image);
        big_image=NULL;
    }

    /* Create the geometry file */
    snprintf(buf,16,"%s.txt",config.basename);
    geom_file = fopen(buf,"w");
    geom_file = freopen(buf,"a",geom_file);

    fprintf(geom_file,"LVLGEOM\n");
    fprintf(geom_file,"Size %u %u\n\n",lvlanim_size,num_entries);
    for (i=0; i<lvlanim_size; i++) {
        fprintf(geom_file,"%s %u {\n",lvlanims[i].name,lvlanims[i].size);
        for (j=0; j<lvlanims[i].size; j++) {
            fprintf(geom_file,"  %5u %5u %5u %5u\n",lvlanims[i].geometry[j].x,lvlanims[i].geometry[j].y,lvlanims[i].geometry[j].w,lvlanims[i].geometry[j].h);
        }
        fprintf(geom_file,"}\n\n");
    }

    /* Summary */
    printf("%s/ Contains: ",config.basename);
    if (config.write&1) printf("1 big animation composition image, ");
    if (config.write&2) printf("%u extracted animation images, ", lvlanim_size);
    if (config.write&4) printf("%u extracted frame images, ", num_entries);
    if (config.debug)   printf("unknown chunk files, ");
    printf("1 geometry files\n");
    printf("Total images: %u, Extracted images: %u, Unknown content: %u bytes\n",tot_entries,num_entries,unknown);

    end:
/*    DestroyMontageInfo(&montage_info);
 *    DestroyMontageInfo(&montage_anim_info);
 */
    if (config.write!=0) {
        if (anim_list)   DestroyImageList(anim_list);
        if (image_list)  DestroyImageList(image_list);
        if (big_image)   DestroyImage(big_image);
        if (image_info)  DestroyImageInfo(image_info);
        if (d_exception) DestroyExceptionInfo(&exception);
    }
                         DestroyMagick();

    if (data)            munmap(data, data_size);
                         free(img_offsets);
    if (lvlanims && lvlanim_size)        freeLVLAnimList(lvlanims,lvlanim_size);
    if (lvl_file!=NULL)  fclose(lvl_file);
    if (geom_file!=NULL) fclose(geom_file);

    return errno;
}
