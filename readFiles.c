#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include "definitions.h"
#include "readDirectories.h"
#include "readFiles.h"


int readFiles(FILE *img, struct superblock sb, struct group_descriptor gdt[], struct inode file_inode){

    //creating a buffer variable to store our data
    char *file = malloc(1024 << sb.s_log_block_size);

    //storing file_inode.i_size in a variable since we need to change the value in the loop
    int bytes_toread = file_inode.i_size, bytes_read;
    int block_size = 1024 << sb.s_log_block_size;


    for(int i = 0; i < 12; i++){
        //break if the current block is empty
        if(file_inode.i_block[i] == 0) continue;

        //moving the file pointer to the start of the current block
        fseek(img, file_inode.i_block[i] * (block_size), SEEK_SET);
        
        if(bytes_toread > block_size){
            bytes_read = fread(file, 1, (block_size), img); //stores the number of bytes read in bytes_read
        }
        else{
            bytes_read = fread(file, 1, bytes_toread, img);
        }

        bytes_toread -= bytes_read;
    }
    printf("%s\n", file);
    free(file);
}