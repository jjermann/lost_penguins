/*
 * (C) 2004 Jonas Jermann
 *
 * Extract Images from .lvl files using ImageMagick api
 *
 * License: GPL
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <magick/api.h>

enum {
    red=0,
    green=1,
    blue=2
};

enum {
    img_x=0,
    img_y=1,
    img_w=2,
    img_h=3
};

enum {
    errno_ok=0,
    errno_quit=1,
    errno_parse=2,
    errno_open=3,
    errno_invalid=4,
    errno_misc=5
};

struct config_s {
    /* create unknown.bin chunk files if != 0 */
    int debug;
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
} config;

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
    printf("Extracts images from file.lvl.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -h, --help             Print this help message\n");
    printf("  -debug <n>             Create additional files if n>0 (default: 0)\n");
    printf("  -colorkey \"#rrggbb\"  Use the specified color for the colorkey. The numbers\n");
    printf("                         rr, gg, bb are hex values from 0 to ff. (default: #ff00ff)\n");
    printf("  -bg \"#rrggbb\"        Use the specified color for the background (default: colorkey)\n");
    printf("  -dir, -o <basename>    Name of the output directory (default: file)\n");
    printf("  -format <format>       Name of the output image format (default: png)\n");
    printf("  -data_file <filename>  Data filename for file.lvl (default: file.dat)\n");
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
                i++;
                if (strncmp(argv[i],"-",1)==0) { usage(); return errno_parse; }
                config.debug=atoi(argv[i]);
            } else if (strcmp(argv[i],"-colorkey")==0) {
                i++;
                if (!strncmp(argv[i],"#",1)==0) { usage(); return errno_parse; }
                strcpy(buf,argv[i]+1); buf[2]=0;
                config.colorkey.red=MaxRGB*strtol(buf,NULL,16)/255;
                strcpy(buf,argv[i]+3); buf[2]=0;
                config.colorkey.green=MaxRGB*strtol(buf,NULL,16)/255;
                strcpy(buf,argv[i]+5); buf[2]=0;
                config.colorkey.blue=MaxRGB*strtol(buf,NULL,16)/255;
                config.bg.blue=config.colorkey.blue;
                config.bg.red=config.colorkey.red;
                config.bg.green=config.colorkey.green;
            } else if (strcmp(argv[i],"-bg")==0) {
                i++;
                if (strncmp(argv[i],"#",1)==0) {
                    strcpy(buf,argv[i]+1); buf[2]=0;
                    config.bg.red=MaxRGB*strtol(buf,NULL,16)/255;
                    strcpy(buf,argv[i]+3); buf[2]=0;
                    config.bg.green=MaxRGB*strtol(buf,NULL,16)/255;
                    strcpy(buf,argv[i]+5); buf[2]=0;
                    config.bg.blue=MaxRGB*strtol(buf,NULL,16)/255;
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
        sprintf(config.data_file,"%s.dat",config.basename);
    }
    sprintf(config.geom_file,"%s.txt",config.basename);

    /* set colorkey in palette */
    lvl_palette[0][red]   = config.colorkey.red*255/MaxRGB;
    lvl_palette[0][green] = config.colorkey.green*255/MaxRGB;
    lvl_palette[0][blue]  = config.colorkey.blue*255/MaxRGB;
    lvl_palette[1][red]   = config.colorkey.red*255/MaxRGB;
    lvl_palette[1][green] = config.colorkey.green*255/MaxRGB;
    lvl_palette[1][blue]  = config.colorkey.blue*255/MaxRGB;

    return errno_ok;
}

/*
 * Create an ImageMagick image of size width x height
 * 
 * image_info is an initialized ImageMagick ImageInfo* ptr
 * ptr is the pointer to the image data offset
 */
Image* write_magick(unsigned char* ptr, ImageInfo* image_info, unsigned int width, unsigned int height) {
    Image* image;
    unsigned int y;
    register unsigned int x;
    register PixelPacket *q;
    register unsigned char *p;

    image=AllocateImage(image_info);
    image->columns=width;
    image->rows=height;  
    image->matte_color=config.colorkey;

    p=ptr;
    for (y=0; y < image->rows; y++) {
        q=SetImagePixels(image,0,y,image->columns,1);
        if (q == (PixelPacket *) NULL) break;
        for (x=0; x < image->columns; x++) { 
            q->red=MaxRGB*(lvl_palette[*p][red])/255;
            q->green=MaxRGB*(lvl_palette[*p][green])/255;
            q->blue=MaxRGB*(lvl_palette[*p][blue])/255; 
            p++;
            q++;
        }
        if (!SyncImagePixels(image)) break;
    }
    return image;
}


int main(int argc, char *argv[]) {
    /* ImageMagick stuff */
    Image* image_list=NULL;
    ImageInfo *image_info=NULL;
    MontageInfo montage_info;
    ExceptionInfo exception;
    Image *big_image, *tmp_img;

    const char entry_id[]="TRPS";
    unsigned int data_size, num_entries, unknown=0;
    FILE *lvl_file, *data_file, *unknown_file, *geom_file;

    /* Initial start pointer to the beginning of the file */
    unsigned char *data;
    /* Temporary variable to indicate the last offset to start searching from:
     * Either after (1+) an invalid "T" or after the image of a valid "TRPS" */
    unsigned char *last_ptr;
    /* Either index position (start) or the last known position after an image */
    unsigned char *save_ptr;
    /* Iterates through all offsets at a "T" */
    unsigned char *off_ptr;

    /* temporary variables */
    char buf[20];
    struct stat sb;
    unsigned int i,width,height,x_off=0;

    /* -------------------------------------------------------------------- */

    /* parse command line options and set config */
    i=parse(argc,argv);

    if (i==errno_quit) return errno_ok;
    else if (i==errno_parse) return errno_parse;
    else if (i!=errno_ok) return i;

    /* Check lvl_file */
    lvl_file = fopen(config.lvl_file,"r");

    if (lvl_file == NULL) {
        printf("Error opening file\n");
        return errno_open;
    }

    fread(buf,1,12,lvl_file);

    if ((buf[0] != 'D') && (buf[1] != 'A') &&
      (buf[2] != 'T') && (buf[3] != 'A')) {
        printf("Invalid file\n");
        fclose(lvl_file);
        return errno_invalid;
    }

    /* get file size */
    fstat(fileno(lvl_file), &sb);
    data_size = sb.st_size;

    /* Check data_file */
    data_file = fopen(config.data_file,"r");

    if (data_file == NULL) printf("No data file %s found!\n",config.data_file);

    /* Change to the base directory */
    mkdir(config.basename, 0777);
    chdir(config.basename);
    printf("Directory: %s\n", config.basename);

    /* Create file for image informations */
    snprintf(buf,16,"%s.txt",config.basename);
    geom_file = fopen(buf,"w");
    geom_file = freopen(buf,"a",geom_file);

    /* ImageMagick stuff */
    image_list=NewImageList(); 
    image_info=CloneImageInfo((ImageInfo *) NULL);
    image_info->colorspace = RGBColorspace;

    /* Map the entire file into process memory space */
    data = mmap(NULL, data_size, PROT_READ, MAP_PRIVATE, fileno(lvl_file), 0);

    /* Iterate through all image data offsets to get the number of entries and the
     * size of the unknown chunk */
    num_entries=0;
    last_ptr=data;
    save_ptr=data;
    while (off_ptr=(unsigned char*)memchr(last_ptr,'T',(data_size-(last_ptr-data)))) {
        unknown+=(off_ptr-last_ptr);
        if (!strncmp((char*)off_ptr,entry_id,4)) {
            num_entries++;
            width=*((unsigned int *)(off_ptr+4));
            height=*((unsigned int *)(off_ptr+8));
            last_ptr=off_ptr+12+width*height;
            save_ptr=last_ptr;
        } else {
            last_ptr=off_ptr+1;
        }
    }

    unsigned short geometry[num_entries][4];

    /* Iterate through all image data offsets */
    i=0;
    last_ptr=data;
    save_ptr=data;
    while (off_ptr=(unsigned char*)memchr(last_ptr,'T',(data_size-(last_ptr-data)))) {
        unknown+=(off_ptr-last_ptr);
        if (!strncmp((char*)off_ptr,entry_id,4)) {
            i++;
            /*
             * Write unknown content (up to this image position (off_ptr)) to a file.
             * If the file size would be 0, skip it.
             */
            if (config.debug && (off_ptr-save_ptr > 0)) {
                snprintf(buf, 16, "%08d.bin", i);
                unknown_file = fopen(buf, "wb");
                fwrite(save_ptr,1,(off_ptr-save_ptr),unknown_file);
                fclose(unknown_file);
            }

            /* --== Parse header ==-- */
            width=*((unsigned int *)(off_ptr+4));
            height=*((unsigned int *)(off_ptr+8));
            geometry[i-1][img_x]=x_off;
            geometry[i-1][img_y]=0;
            geometry[i-1][img_w]=width;
            geometry[i-1][img_h]=height;
            x_off+=width;

            /* update last_ptr, get the image and append it to the list */
            tmp_img=write_magick(off_ptr+12,image_info,width,height);
            snprintf(buf,16,"%08d.%s",i,config.format);
            strcpy(tmp_img->filename,buf);
            AppendImageToList(&image_list,tmp_img);

            /* update pointers */
            last_ptr=off_ptr+12+width*height;
            save_ptr=last_ptr;
        } else {
            last_ptr=off_ptr+1;
        }
    }

    /* Montage the image and save it */
    GetMontageInfo(image_info,&montage_info);
    GetExceptionInfo(&exception);
    montage_info.tile="3000x1";
    montage_info.geometry="+0+0";
    montage_info.background_color=config.bg;
    montage_info.matte_color=config.colorkey;
    montage_info.gravity=NorthWestGravity;

    big_image=MontageImages(image_list,&montage_info,&exception);
    big_image->matte_color=config.colorkey;

    snprintf(buf, 16, "%s.%s",config.basename,config.format);
    strcpy(big_image->filename,buf);
    WriteImage(image_info,big_image);
    DestroyImage(big_image);

    /* Create geom_file */
    fprintf(geom_file,"%d\n",num_entries);
    for (i=0; i<num_entries;i++) {
        fprintf(geom_file,"%d %d %d %d\n",geometry[i][0],geometry[i][1],geometry[i][2],geometry[i][3]);
    }

    /* Summary */
    printf("Contains %d extracted images, unknown content: %d bytes\n",num_entries,unknown);

    /* Cleanup */
/*
    DestroyMontageInfo(&montage_info);
*/
    DestroyImageList(image_list);
    DestroyImageInfo(image_info);
    DestroyExceptionInfo(&exception);
    DestroyMagick();

    munmap(data, data_size);
    if (lvl_file!=NULL)  fclose(lvl_file);
    if (geom_file!=NULL) fclose(geom_file);
    if (data_file!=NULL) fclose(data_file);

    return errno_ok;
}
