/*
 * (C) 2004 Alex Beregszaszi
 *
 * License: GPL
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h> 
#include <stdlib.h>   
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void put_wav_header(int fd, int samp, int bit, int chans, int size) {
    int bps = chans*samp;
    char buf[44] = { 'R','I','F','F', 0,0,0,0, 'W','A','V','E',
            'f','m','t',' ', 0,0,0,0,
            1,0, 0,0, 0,0,0,0, 0,0,0,0, 0,0, 8,0,
            'd','a','t','a', 0,0,0,0 };

    buf[4] = (size + 36);
    buf[5] = (size + 36) >> 8;
    buf[6] = (size + 36) >> 16;
    buf[7] = (size + 36) >> 24;
    
    buf[16] = 16;
    buf[17] = 16 >> 8;
    buf[18] = 16 >> 16;
    buf[19] = 16 >> 24;

    buf[22] = chans;
    buf[23] = chans >> 8;
    
    buf[24] = samp;
    buf[25] = samp >> 8;
    buf[26] = samp >> 16;
    buf[27] = samp >> 24;
    
    buf[28] = bps;
    buf[29] = bps >> 8;
    buf[30] = bps >> 16;
    buf[31] = bps >> 24;

    buf[32] = chans;
    buf[33] = chans >> 8;

    buf[42] = size;
    buf[43] = size >> 8;
    buf[44] = size >> 16;
    buf[45] = size >> 24;

    write(fd, buf, 44);
}

int read_entry(int fd_in) {
    int swapped = 0, size, samp, bits, chans;
    char filename[14], buf[512];
    int fd_out, pos = 0;

    printf("new entry at %x\n", (unsigned int)lseek(fd_in, 0, SEEK_CUR));

    /* header */
    read(fd_in, buf, 70);
    /* byte swapped entry */
    if (buf[3]) swapped++;    

    /* FIXME: size could be entries aswell? get some non-8bit samples */

    if (swapped) {
        size = (buf[0] << 16) | (buf[1] << 24) | (buf[2]) | (buf[3] << 8);
        samp = (buf[4] << 16) | (buf[5] << 24) | (buf[6]) | (buf[7] << 8);
        bits = (buf[8] << 16) | (buf[9 ] << 24) | (buf[10]) | (buf[11] << 8);
        chans = (buf[12] << 16)| (buf[13] << 24) | (buf[14]) | (buf[15] << 8);
    
        size += 2;

        strncpy(&filename[0], &buf[52], 14);
    } else {
        size = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
        samp = buf[4] | (buf[5] << 8) | (buf[6] << 16) | (buf[7] << 24);
        bits = buf[8] | (buf[9] << 8) | (buf[10] << 16) | (buf[11] << 24);
        chans = buf[12] | (buf[13] << 8) | (buf[14] << 16) | (buf[15] << 24);

        strncpy(&filename[0], &buf[50], 14);
    }
    
    if (!size) return -1;

    printf("entry %x: %s size: %x %d/%d/%d\n", (unsigned int)lseek(fd_in, 0, SEEK_CUR), filename, size, samp, bits, chans);

    strncpy(buf, filename, 14);
    if (strrchr(buf, '.')) {
        char *loc = strrchr(buf, '.');
        strncpy(loc, ".wav", 5);
    }
    printf("Writing out %s...\n", buf);

    fd_out = open(buf, O_WRONLY|O_CREAT, 0666);
    put_wav_header(fd_out, samp, bits, chans, size);
    
    while(pos < size) {
        int ret = read(fd_in, buf, (size-pos > 512) ? 512 : size-pos);
    
        write(fd_out, buf, ret);
        pos += ret;
    }
    
    close(fd_out);
    
    if (size % 4) {
        int aligned = size + (4 - (size % 4));
    
        printf("aligned: %d, pos: %d, %d\n", aligned, pos, aligned-pos);
        lseek(fd_in, aligned-pos, SEEK_CUR);
    
        return 70 + size + aligned-pos;
    }

    return 70 + size;
}

int main(int argc, char *argv[]) {
    int fd_in;
    char buf[16];
    int size, pos = 0, counter = 0;

    if (argc < 2) {
        printf("Usage: %s input.slv [destination]\n", argv[0]);
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
    
    size = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];
    printf("size: %d\n", size);

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
    
    while(pos < size) {
        int tmp = read_entry(fd_in);
        if (tmp == -1) break;
        pos += tmp;
        counter++;
    }
    
    printf("extracted %d items\n", counter);
    return 0;
}
