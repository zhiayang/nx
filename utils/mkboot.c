/*
 * x86_64-bios/mkboot.c
 *
 * Copyright (C) 2017 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This file is part of the BOOTBOOT Protocol package.
 * @brief Little tool to install boot.bin in MBR or VBR
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

/* the BOOTBOOT 1st stage loader code */
extern unsigned char *_binary____boot_bin_start;

/* entry point */
int main(int argc, char** argv)
{
    // variables
    unsigned char bootrec[512], data[512];
    int f, lba=0, seek=0, lsn, bootlsn=-1;

    // check arguments
    if(argc < 2) {
        printf( "BOOTBOOT mkboot utility - bztsrc@gitlab\n\nUsage:\n"
                "  ./mkboot <disk> [partition lba] [bootsector lba]\n\n"
                "Installs boot record on a disk. Disk can be a local file, a disk or partition\n"
                "device. If you want to install it on a partition, you'll have to specify the\n"
                "starting LBA of that partition as well. Requires that bootboot.bin is already\n"
                "copied on the disk in a contiguous area in order to work.\n\n"
                "Examples:\n"
                "  ./mkboot diskimage.dd      - installing on a disk image\n"
                "  ./mkboot /dev/sda          - installing as (P)MBR\n"
                "  ./mkboot /dev/sda 123      - installing as VBR on a disk device\n"
                "  ./mkboot /dev/sda1 123 0   - installing as VBR on a partition device\n");
        return 1;
    }
    if(argc > 2 || argv[2]!=NULL) {
        lba = seek = atoi(argv[2]);
    }
    if(argc > 3 || argv[3]!=NULL) {
        seek = atoi(argv[3]);
    }
    if(lba < seek) seek = lba;
    // open file
    f = open(argv[1], O_RDONLY);
    if(f < 0) {
        fprintf(stderr, "mkboot: file not found\n");
        return 2;
    }
    // read the boot record
    if(seek>0) lseek(f, seek*512, SEEK_SET);
    if(read(f, data, 512)==-1) {
        close(f);
        fprintf(stderr, "mkboot: unable to read file\n");
        return 2;
    }
    // create the boot record. First copy the code then the data area from original sector on disk
    memcpy((void*)&bootrec, (void*)&_binary____boot_bin_start, 512);
    memcpy((void*)&bootrec+0xB, (void*)&data+0xB, 0x5A-0xB);        // copy BPB (if any)
    memcpy((void*)&bootrec+0x1B8, (void*)&data+0x1B8, 510-0x1B8);   // copy WNTID and partitioning table (if any)
    // now locate the second stage by magic bytes
    for(lsn = 1; lsn < 1024*1024; lsn++) {
        printf("Checking sector %d\r", lsn);
        if(read(f, data, 512) != -1 &&
            data[0] == 0x55 && data[1] == 0xAA && data[3] == 0xE9 && data[8] == 'B' && data[12] == 'B') {
                bootlsn=lsn;
                break;
        }
    }
    close(f);
    if(bootlsn == -1) {
        fprintf(stderr, "mkboot: unable to locate 2nd stage (bootboot.bin) in the first 512 Mbyte\n");
        return 2;
    }
    // add bootboot.bin's address to boot record
    bootlsn += lba - seek;
    memcpy((void*)&bootrec+0x1B0, (void*)&bootlsn, 4);
    // save boot record
    f = open(argv[1], O_WRONLY);
    if(seek>0 && f) lseek(f, seek*512, SEEK_SET);
    if(f < 0 || write(f, bootrec, 512) <= 0) {
        fprintf(stderr, "mkboot: unable to write boot record\n");
        return 3;
    }
    close(f);
    // all went well
    printf("mkboot: BOOTBOOT installed, 2nd stage starts at LBA %d\n", bootlsn);
}
