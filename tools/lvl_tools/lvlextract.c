/*
 * (C) 2004 Jonas Jermann
 *
 * License: GPL
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "lvlextract.h"

/* RGB(A) palette */
unsigned short lvl_palette[256][3]= {
/* (One of) the first two RGB values is the background
 * (colorkey) color... in lost_penguins it's set to
 * 0xff00ff
 */
/*
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
*/
    0xff, 0x00, 0xff, 
    0xff, 0x00, 0xff, 

    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x08, 0x04, 0x00, 
    0x00, 0x04, 0x10, 
    0x08, 0x10, 0x00, 
    0x08, 0x10, 0x10, 
    0x18, 0x10, 0x00, 
    0x18, 0x14, 0x10, 
    0x00, 0x08, 0x29, 
    0x00, 0x0c, 0x39, 
    0x10, 0x14, 0x20, 
    0x18, 0x18, 0x20, 
    0x08, 0x20, 0x08, 
    0x08, 0x24, 0x29, 
    0x18, 0x28, 0x39, 
    0x29, 0x10, 0x08, 
    0x20, 0x24, 0x00, 
    0x20, 0x24, 0x10, 
    0x31, 0x24, 0x00, 
    0x31, 0x24, 0x00, 
    0x31, 0x34, 0x00, 
    0x31, 0x34, 0x10, 
    0x20, 0x20, 0x20, 
    0x29, 0x24, 0x20, 
    0x20, 0x34, 0x29, 
    0x31, 0x30, 0x29, 
    0x31, 0x30, 0x39, 
    0x00, 0x0c, 0x4a, 
    0x62, 0x65, 0x08, 
    0x73, 0x6d, 0x52, 
    0x73, 0x00, 0xb4, 
    0x94, 0x38, 0x20, 
    0xa4, 0x65, 0x39, 
    0x31, 0x34, 0x4a, 
    0x29, 0x34, 0x62, 
    0x08, 0x44, 0x10, 
    0x00, 0x65, 0x00, 
    0x29, 0x59, 0x39, 
    0x31, 0x7d, 0x39, 
    0x29, 0x48, 0x6a, 
    0x4a, 0x0c, 0x08, 
    0x41, 0x2c, 0x00, 
    0x41, 0x28, 0x10, 
    0x52, 0x2c, 0x10, 
    0x52, 0x3c, 0x10, 
    0x4a, 0x38, 0x29, 
    0x41, 0x3c, 0x39, 
    0x62, 0x1c, 0x08, 
    0x73, 0x08, 0x08, 
    0x62, 0x3c, 0x10, 
    0x7b, 0x20, 0x08, 
    0x73, 0x34, 0x10, 
    0x41, 0x3c, 0x4a, 
    0x41, 0x40, 0x00, 
    0x41, 0x40, 0x10, 
    0x4a, 0x55, 0x00, 
    0x52, 0x40, 0x00, 
    0x52, 0x55, 0x08, 
    0x52, 0x55, 0x18, 
    0x52, 0x44, 0x39, 
    0x5a, 0x59, 0x39, 
    0x62, 0x44, 0x00, 
    0x62, 0x55, 0x00, 
    0x62, 0x55, 0x18, 
    0x73, 0x44, 0x10, 
    0x73, 0x59, 0x00, 
    0x73, 0x59, 0x10, 
    0x62, 0x40, 0x20, 
    0x62, 0x44, 0x39, 
    0x73, 0x40, 0x20, 
    0x7b, 0x44, 0x20, 
    0x7b, 0x55, 0x20, 
    0x7b, 0x59, 0x31, 
    0x62, 0x65, 0x08, 
    0x62, 0x65, 0x18, 
    0x62, 0x75, 0x18, 
    0x7b, 0x6d, 0x08, 
    0x73, 0x6d, 0x18, 
    0x7b, 0x7d, 0x08, 
    0x7b, 0x7d, 0x18, 
    0x7b, 0x6d, 0x29, 
    0x7b, 0x7d, 0x29, 
    0x4a, 0x48, 0x52, 
    0x52, 0x50, 0x52, 
    0x4a, 0x4c, 0x6a, 
    0x4a, 0x4c, 0x7b, 
    0x5a, 0x59, 0x62, 
    0x4a, 0x71, 0x5a, 
    0x6a, 0x55, 0x52, 
    0x73, 0x6d, 0x52, 
    0x62, 0x65, 0x62, 
    0x62, 0x6d, 0x7b, 
    0x73, 0x71, 0x6a, 
    0x7b, 0x79, 0x7b, 
    0x08, 0x10, 0x94, 
    0x00, 0x04, 0xac, 
    0x10, 0x1c, 0xa4, 
    0x18, 0x20, 0xbd, 
    0x20, 0x30, 0x83, 
    0x00, 0x04, 0xf6, 
    0x29, 0x30, 0xc5, 
    0x29, 0x4c, 0x83, 
    0x20, 0x40, 0xa4, 
    0x20, 0x55, 0xc5, 
    0x20, 0x4c, 0xee, 
    0x73, 0x00, 0xb4, 
    0x4a, 0x4c, 0xac, 
    0x4a, 0x71, 0x83, 
    0x52, 0x6d, 0xac, 
    0x4a, 0x48, 0xcd, 
    0x52, 0x4c, 0xee, 
    0x5a, 0x6d, 0xe6, 
    0x00, 0x99, 0x00, 
    0x10, 0xea, 0x08, 
    0x4a, 0x85, 0x39, 
    0x4a, 0xa5, 0xac, 
    0x7b, 0x85, 0xa4, 
    0x5a, 0xba, 0xee, 
    0x6a, 0x89, 0xee, 
    0x94, 0x08, 0x08, 
    0x94, 0x2c, 0x08, 
    0x94, 0x38, 0x20, 
    0xac, 0x0c, 0x08, 
    0xb4, 0x30, 0x10, 
    0xb4, 0x34, 0x31, 
    0x94, 0x50, 0x10, 
    0x8b, 0x4c, 0x20, 
    0x8b, 0x5d, 0x20, 
    0x8b, 0x5d, 0x31, 
    0x8b, 0x69, 0x00, 
    0x9c, 0x69, 0x08, 
    0x94, 0x7d, 0x00, 
    0x94, 0x79, 0x39, 
    0xac, 0x55, 0x00, 
    0xa4, 0x59, 0x20, 
    0xa4, 0x5d, 0x31, 
    0xbd, 0x50, 0x20, 
    0xa4, 0x65, 0x39, 
    0xa4, 0x7d, 0x20, 
    0xac, 0x75, 0x39, 
    0xbd, 0x65, 0x29, 
    0xbd, 0x6d, 0x39, 
    0x8b, 0x61, 0x4a, 
    0x9c, 0x79, 0x5a, 
    0xd5, 0x08, 0x08, 
    0xd5, 0x30, 0x20, 
    0xf6, 0x08, 0x08, 
    0xf6, 0x28, 0x20, 
    0xf6, 0x38, 0x39, 
    0xc5, 0x59, 0x39, 
    0xc5, 0x79, 0x29, 
    0xc5, 0x7d, 0x39, 
    0xde, 0x7d, 0x31, 
    0xe6, 0x59, 0x29, 
    0xe6, 0x59, 0x52, 
    0xff, 0x59, 0x52, 
    0xb4, 0x24, 0xbd, 
    0x8b, 0x81, 0x10, 
    0x94, 0x95, 0x00, 
    0x8b, 0x81, 0x20, 
    0x9c, 0x99, 0x20, 
    0xa4, 0x81, 0x00, 
    0xa4, 0x81, 0x10, 
    0xac, 0x9d, 0x00, 
    0xbd, 0x81, 0x08, 
    0xac, 0x99, 0x20, 
    0xb4, 0xae, 0x18, 
    0xa4, 0xaa, 0x20, 
    0xb4, 0xae, 0x29, 
    0xbd, 0xb6, 0x39, 
    0x94, 0x81, 0x73, 
    0xa4, 0x99, 0x41, 
    0xbd, 0x99, 0x5a, 
    0xb4, 0x8d, 0x73, 
    0xbd, 0xaa, 0x73, 
    0xc5, 0x9d, 0x00, 
    0xcd, 0x99, 0x20, 
    0xcd, 0xb6, 0x00, 
    0xc5, 0xb6, 0x29, 
    0xd5, 0xba, 0x20, 
    0xde, 0xba, 0x39, 
    0xff, 0x99, 0x00, 
    0xe6, 0xae, 0x00, 
    0xc5, 0x81, 0x4a, 
    0xc5, 0x95, 0x41, 
    0xd5, 0x99, 0x41, 
    0xd5, 0x99, 0x52, 
    0xde, 0x85, 0x73, 
    0xd5, 0xaa, 0x5a, 
    0xd5, 0xb2, 0x6a, 
    0xd5, 0xb6, 0x7b, 
    0xe6, 0x81, 0x52, 
    0xf6, 0x85, 0x73, 
    0xee, 0xa5, 0x5a, 
    0xf6, 0xb2, 0x73, 
    0xd5, 0xce, 0x00, 
    0xd5, 0xca, 0x31, 
    0xd5, 0xd2, 0x20, 
    0xde, 0xda, 0x39, 
    0xf6, 0xd2, 0x00, 
    0xff, 0xf2, 0x00, 
    0xee, 0xee, 0x20, 
    0xf6, 0xf2, 0x39, 
    0xd5, 0xca, 0x41, 
    0xde, 0xda, 0x4a, 
    0xee, 0xde, 0x52, 
    0xf6, 0xde, 0x73, 
    0xee, 0xe2, 0x41, 
    0xf6, 0xf2, 0x52, 
    0x8b, 0x89, 0x83, 
    0x94, 0x95, 0x9c, 
    0x8b, 0x99, 0xb4, 
    0xb4, 0xa5, 0x94, 
    0xa4, 0xa5, 0xac, 
    0xb4, 0xb2, 0xb4, 
    0xbe, 0xbd, 0xbe, 
    0x94, 0x91, 0xf6, 
    0xac, 0xb6, 0xd5, 
    0xac, 0xb2, 0xff, 
    0xbd, 0xd2, 0xee, 
    0xd5, 0xb6, 0x94, 
    0xcd, 0xba, 0xb4, 
    0xf6, 0x99, 0x94, 
    0xf6, 0xba, 0x9c, 
    0xde, 0xca, 0x8b, 
    0xde, 0xce, 0xa4, 
    0xde, 0xd2, 0xb4, 
    0xff, 0xde, 0x94, 
    0xf6, 0xda, 0xbd, 
    0xff, 0xfa, 0xbd, 
    0xcd, 0xca, 0xcd, 
    0xde, 0xda, 0xcd, 
    0xde, 0xda, 0xde, 
    0xd5, 0xf6, 0xff, 
    0xe6, 0xe2, 0xee, 
    0xf6, 0xf2, 0xe6, 
    0xff, 0xfa, 0xff, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 
};

/* unused */
unsigned int lvl_palette2[256]= {
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x08040000, 0x00041000, 0x08100000, 0x08101000,
    0x18100000, 0x18141000, 0x00082900, 0x000c3900, 0x10142000, 0x18182000, 0x08200800, 0x08242900, 0x18283900, 0x29100800, 0x20240000, 0x20241000, 0x31240000, 0x31240000, 0x31340000, 0x31341000,
    0x20202000, 0x29242000, 0x20342900, 0x31302900, 0x31303900, 0x000c4a00, 0x62650800, 0x736d5200, 0x7300b400, 0x94382000, 0xa4653900, 0x31344a00, 0x29346200, 0x08441000, 0x00650000, 0x29593900,
    0x317d3900, 0x29486a00, 0x4a0c0800, 0x412c0000, 0x41281000, 0x522c1000, 0x523c1000, 0x4a382900, 0x413c3900, 0x621c0800, 0x73080800, 0x623c1000, 0x7b200800, 0x73341000, 0x413c4a00, 0x41400000,
    0x41401000, 0x4a550000, 0x52400000, 0x52550800, 0x52551800, 0x52443900, 0x5a593900, 0x62440000, 0x62550000, 0x62551800, 0x73441000, 0x73590000, 0x73591000, 0x62402000, 0x62443900, 0x73402000,
    0x7b442000, 0x7b552000, 0x7b593100, 0x62650800, 0x62651800, 0x62751800, 0x7b6d0800, 0x736d1800, 0x7b7d0800, 0x7b7d1800, 0x7b6d2900, 0x7b7d2900, 0x4a485200, 0x52505200, 0x4a4c6a00, 0x4a4c7b00,
    0x5a596200, 0x4a715a00, 0x6a555200, 0x736d5200, 0x62656200, 0x626d7b00, 0x73716a00, 0x7b797b00, 0x08109400, 0x0004ac00, 0x101ca400, 0x1820bd00, 0x20308300, 0x0004f600, 0x2930c500, 0x294c8300,
    0x2040a400, 0x2055c500, 0x204cee00, 0x7300b400, 0x4a4cac00, 0x4a718300, 0x526dac00, 0x4a48cd00, 0x524cee00, 0x5a6de600, 0x00990000, 0x10ea0800, 0x4a853900, 0x4aa5ac00, 0x7b85a400, 0x5abaee00,
    0x6a89ee00, 0x94080800, 0x942c0800, 0x94382000, 0xac0c0800, 0xb4301000, 0xb4343100, 0x94501000, 0x8b4c2000, 0x8b5d2000, 0x8b5d3100, 0x8b690000, 0x9c690800, 0x947d0000, 0x94793900, 0xac550000,
    0xa4592000, 0xa45d3100, 0xbd502000, 0xa4653900, 0xa47d2000, 0xac753900, 0xbd652900, 0xbd6d3900, 0x8b614a00, 0x9c795a00, 0xd5080800, 0xd5302000, 0xf6080800, 0xf6282000, 0xf6383900, 0xc5593900,
    0xc5792900, 0xc57d3900, 0xde7d3100, 0xe6592900, 0xe6595200, 0xff595200, 0xb424bd00, 0x8b811000, 0x94950000, 0x8b812000, 0x9c992000, 0xa4810000, 0xa4811000, 0xac9d0000, 0xbd810800, 0xac992000,
    0xb4ae1800, 0xa4aa2000, 0xb4ae2900, 0xbdb63900, 0x94817300, 0xa4994100, 0xbd995a00, 0xb48d7300, 0xbdaa7300, 0xc59d0000, 0xcd992000, 0xcdb60000, 0xc5b62900, 0xd5ba2000, 0xdeba3900, 0xff990000,
    0xe6ae0000, 0xc5814a00, 0xc5954100, 0xd5994100, 0xd5995200, 0xde857300, 0xd5aa5a00, 0xd5b26a00, 0xd5b67b00, 0xe6815200, 0xf6857300, 0xeea55a00, 0xf6b27300, 0xd5ce0000, 0xd5ca3100, 0xd5d22000,
    0xdeda3900, 0xf6d20000, 0xfff20000, 0xeeee2000, 0xf6f23900, 0xd5ca4100, 0xdeda4a00, 0xeede5200, 0xf6de7300, 0xeee24100, 0xf6f25200, 0x8b898300, 0x94959c00, 0x8b99b400, 0xb4a59400, 0xa4a5ac00,
    0xb4b2b400, 0xbebdbe00, 0x9491f600, 0xacb6d500, 0xacb2ff00, 0xbdd2ee00, 0xd5b69400, 0xcdbab400, 0xf6999400, 0xf6ba9c00, 0xdeca8b00, 0xdecea400, 0xded2b400, 0xffde9400, 0xf6dabd00, 0xfffabd00,
    0xcdcacd00, 0xdedacd00, 0xdedade00, 0xd5f6ff00, 0xe6e2ee00, 0xf6f2e600, 0xfffaff00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};

char* xpm_map[256] = {
    "  ", ". ", "+ ", "@ ", "# ", "$ ", "% ", "& ", "* ", "= ", "- ", "; ", "> ", ", ", "' ", ") ",
    "! ", "~ ", "{ ", "] ", "^ ", "/ ", "( ", "_ ", ": ", "< ", "[ ", "} ", "| ", "1 ", "2 ", "3 ",
    "4 ", "5 ", "6 ", "7 ", "8 ", "9 ", "0 ", "a ", "b ", "c ", "d ", "e ", "f ", "g ", "h ", "i ",
    "j ", "k ", "l ", "m ", "n ", "o ", "p ", "q ", "r ", "s ", "t ", "u ", "v ", "w ", "x ", "y ",
    "z ", "A ", "B ", "C ", "D ", "E ", "F ", "G ", "H ", "I ", "J ", "K ", "L ", "M ", "N ", "O ",
    "P ", "Q ", "R ", "S ", "T ", "U ", "V ", "W ", "X ", "Y ", "Z ", "` ", " .", "..", "+.", "@.",
    "#.", "$.", "%.", "&.", "*.", "=.", "-.", ";.", ">.", ",.", "\'.", ").", "!.", "~.", "{.", "].",
    "^.", "/.", "(.", "_.", ":.", "<.", "[.", "}.", "|.", "1.", "2.", "3.", "4.", "5.", "6.", "7.",
    "8.", "9.", "0.", "a.", "b.", "c.", "d.", "e.", "f.", "g.", "h.", "i.", "j.", "k.", "l.", "m.",
    "n.", "o.", "p.", "q.", "r.", "s.", "t.", "u.", "v.", "w.", "x.", "y.", "z.", "A.", "B.", "C.",
    "D.", "E.", "F.", "G.", "H.", "I.", "J.", "K.", "L.", "M.", "N.", "O.", "P.", "Q.", "R.", "S.",
    "T.", "U.", "V.", "W.", "X.", "Y.", "Z.", "`.", " +", ".+", "++", "@+", "#+", "$+", "%+", "&+",
    "*+", "=+", "-+", ";+", ">+", ",+", "'+", ")+", "!+", "~+", "{+", "]+", "^+", "/+", "(+", "_+",
    ":+", "<+", "[+", "}+", "|+", "1+", "2+", "3+", "4+", "5+", "6+", "7+", "8+", "9+", "0+", "a+",
    "b+", "c+", "d+", "e+", "f+", "g+", "h+", "i+", "j+", "k+", "l+", "m+", "n+", "o+", "p+", "q+",
    "r+", "s+", "t+", "u+", "v+", "w+", "x+", "y+", "z+", "A+", "B+", "C+", "D+", "E+", "F+", "G+"
};


int main(int argc, char *argv[]) {
    int fd_in;
/*    FILE* append_file; */
    char buf[16];
    char *data;
    char *last_ptr;
    char *off_ptr;
    struct stat sb;
    int data_size;
    int counter=0;
    int unknown=0;
    char entry_id[]="TRPS";

    if (argc < 2) {
    	printf("Usage: %s input.lvl [destination]\n", argv[0]);
	    exit(1);
    }
    
    fd_in = open(argv[1], O_RDONLY);
    
    if (fd_in < 0) {
    	perror("error opening file");
	    exit(1);
    }

    read(fd_in, buf, 12);

    if ((buf[0] != 'D') && (buf[1] != 'A') &&
        (buf[2] != 'T') && (buf[3] != 'A')) {
        printf("Invalid file\n");
        close(fd_in);
        exit(1);
    }

    if (argc > 2) {
        mkdir(argv[2], 0777);
        chdir(argv[2]);
        printf("directory: %s\n", argv[2]);
    } else {
        /* copy the filename */
        if (strrchr(argv[1], '/')) strncpy(buf, strrchr(argv[1], '/')+1, 16);
        else strncpy(buf, argv[1], 16);

        /* truncate filename at . */
        if (strrchr(buf, '.')) {   
            char *loc = strrchr(buf, '.');
            *loc = 0;
        }

        mkdir(buf, 0777);
        chdir(buf);

        printf("directory: %s\n", buf);
    }

    /* get file size */
    fstat(fd_in, &sb);
    data_size = sb.st_size;

    /* File for unknown content */
/*    append_file = fopen("unknown.bin", "a"); */

    /* map the entire file into process memory space */
    data = mmap(NULL, data_size, PROT_READ, MAP_PRIVATE, fd_in, 0);
    last_ptr=data;
    while (off_ptr=memchr(last_ptr,'T',(data_size-(last_ptr-data)))) {
/*
        if (!strncmp(off_ptr,entry_id,4)) fprintf(append_file,"****************");
        fwrite(last_ptr,1,(off_ptr-last_ptr),append_file);
*/
        unknown+=(off_ptr-last_ptr);
        if (!strncmp(off_ptr,entry_id,4)) {
            counter++;
            /* write_pgm, write_png, write_xpm, write_png_xpm */
            last_ptr=write_xpm(off_ptr,counter);
        } else {
            last_ptr=off_ptr+1;
        }
    }

    printf("Contains %d extracted images, unknown content: %d bytes\n",counter,unknown);

    munmap(data, data_size);
    close(fd_in);
/*  fclose(append_file); */

    return 0;
}

