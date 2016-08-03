/*
 * =====================================================================================
 *
 *       Filename:  cpbtool.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013年05月07日 18时55分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  linkscue (scue), 
 *   Organization:  ATX风雅组
 *
 * =====================================================================================
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>

typedef struct {
    unsigned char cp_magic[4];                  /* coolpad file magic */
    unsigned char cp_version[32];               /* coolpad head version */
    unsigned char file_form[256];               /* where the rom come from */
    unsigned char flag_p2[16];                  /* alway is P2 string */
    unsigned char information[12];              /* some information, but unkown */
    unsigned char model[32];                    /* coolpad phone model */
    unsigned char reverse[128];                 /* never use, remain for future */
    unsigned char version[64];                  /* phone version or rom name */
    unsigned int checksum;                      /* here maybe is a checksum */
    unsigned int cpb_filesize;                  /* the size of whole cpb file */
    unsigned int image_offset;                  /* entrance offset of image */
} cpb_head;

typedef struct {                                /* 76 bytes */
    unsigned char filename[32];                 /* image filename */
    unsigned int image_offset;                  /* image offset */
    unsigned int image_size;                    /* image filesize */
    unsigned int checksum;                      /* here maybe is a checksum */
} image_t;

void splitFile(char *file) {
    FILE *fd;
    FILE *ft;
    int i = 0, imagecount = 0;
    cpb_head header;
    image_t images[11];
    printf("\n Welcome to use unpackcpb tool by scue@ATX(bbs.anzhi.com), 2013-05-09, weibo.com/scue.\n");

    /* If file is corrupted */
    if ((fd = fopen(file,"rb")) == NULL) {
        printf ( "Extract cpb file, open %s failure!\n", file );
        exit(1);
    }

    /* Copy each image details to the image_t struct */
    fread(&header, sizeof(header), 1, fd);
    printf("debug : CPB INFO :     %s \n", file);
    printf("debug : cp_magic :     %s \n", header.cp_magic);
    printf("debug : cp_version :   %s \n", header.cp_version);
    printf("debug : file_form:     %s \n", header.file_form);
    printf("debug : flag_p2 :      %s \n", header.flag_p2);
    printf("debug : information :  %s \n", header.information);
    printf("debug : model :        %s \n", header.model);
    //printf("debug : reverse :      %s \n", header.reverse);
    printf("debug : checksum :     %d \n", header.checksum);
    printf("debug : cpb_filesize : %d \n", header.cpb_filesize);
    printf("debug : image_offset :  %d \n", header.image_offset);

    for (i = 0; (ftell(fd) < (header.image_offset)); i++ ) {
        fread(&images[i], sizeof(images[i]), 1, fd);
	strcpy(images[i].filename , "dummy"); // Reset filename
        printf("debug: Image %d filename=%s image_offset=%u image_size=%u checksum=%u \n", i, images[i].filename, images[i].image_offset, images[i].image_size, images[i].checksum);
        imagecount++;
    }

    int size = 0, n = 0, count = 0, offset = 0;
    unsigned char imagename[32] = "";
    unsigned char buffer[4] = "";

    for (i = 0; i < imagecount; i++) {
	// Copy filename
        strncpy(imagename, images[i].filename, sizeof(imagename));
	printf("debug: imagename %s", imagename);
        offset = images[i].image_offset;  // Store image size
        size = images[i].image_size; // Store image offset
        if ( size != 0 ) {
	    /* If file is corrupted */
            if ((ft = fopen(imagename,"wb")) == NULL ){
                printf("Extract cpb file, open %s failure!\n",imagename);
            }
	    // Seek to the right offset (TODO: Tweak this SEEK_SET)
            fseek(fd, offset, SEEK_SET);
            printf("Extract: %-15s offset: 0x%08x  size: %d\n",imagename, offset, size);
            n = 0, count = 0;
            while (count < size)  {
                n = fread(buffer, 1 , sizeof(buffer), fd);
                fwrite(buffer, n, 1, ft);
                count += n;
            }
        }
    }
    fclose(fd);
}

int main (int argc, char *argv[]) {
    if (argc==1) {
        printf("usage:%s cpb file.\n", argv[0]);
        exit(0);
    }
    char *cpb;
    cpb = argv[1];
    splitFile(cpb);
    return EXIT_SUCCESS;
}
