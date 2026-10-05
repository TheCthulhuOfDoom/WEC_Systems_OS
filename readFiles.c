#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include "definitions.h"
#include "readDirectories.h"
#include "readFiles.h"


int readFiles(FILE *img, struct superblock sb, struct group_descriptor gdt[], struct inode file_inode){
    // printf("noto\n");

    //storing file_inode.i_size in a variable since we need to change the value in the loop
    int bytes_toread = file_inode.i_size;
    int block_size = 1024 << sb.s_log_block_size;

    //for the first 12 block_ids
    int i = 0;
    while(i < 12 && bytes_toread > 0){
        //break if the current block is empty
        if(file_inode.i_block[i] == 0) {
            i++;
            continue;
        }

        bytes_toread = readBlock(img, file_inode.i_block[i], block_size, bytes_toread);
        i++;
    }

    //calling readSingly
    if (bytes_toread > 0) {
        bytes_toread = readSingly(img, file_inode.i_block[12], block_size, bytes_toread);
    }

    //calling readDoubly
    if (bytes_toread > 0) {
        bytes_toread = readDoubly(img, file_inode.i_block[13], block_size, bytes_toread);
    }

    //calling readTriply
    if (bytes_toread > 0) {
        bytes_toread = readTriply(img, file_inode.i_block[14], block_size, bytes_toread);
    }

    return 0;
}

int readBlock(FILE *img, int block_id, int block_size, int bytes_toread){
    //creating a buffer variable to store our data
    char *block = malloc(block_size + 1);

    int bytes_read;

    //moving the file pointer to the start of the current block
    fseek(img, block_id * (block_size), SEEK_SET);
    if(bytes_toread > block_size){
        bytes_read = fread(block, 1, (block_size), img); //stores the number of bytes read in bytes_read
    }
    else{
        bytes_read = fread(block, 1, bytes_toread, img);
    }

    bytes_toread -= bytes_read;
    // fwrite(file, 1, bytes_read, stdout);
    block[block_size] = '\0';
    printf("%s\n", block);
    free(block);

    return bytes_toread;
}

int readSingly(FILE *img, int singly_block_id, int block_size, int bytes_toread){
    //for the singly indirect blocks
    

    int block_id;
    int offset = singly_block_id * block_size;

    while(offset < block_size && bytes_toread > 0){
        //moving the file_pointer to offset
        fseek(img, offset, SEEK_SET);
        //reading a direct block id from the singly indirect block
        fread(&block_id, 1, 4, img);
        offset += 4;

        //calling readBlock
        bytes_toread = readBlock(img, block_id, bytes_toread, block_size);
    }

    return bytes_toread;
}

int readDoubly(FILE *img, int doubly_block_id, int block_size, int bytes_toread){
    //for the doubly indirect blocks
    

    int singly_block_id;
    int offset = doubly_block_id * block_size;

    while(offset < block_size && bytes_toread > 0){
        //moving the file_pointer to offset
        fseek(img, offset, SEEK_SET);
        //reading a singly indirect block id from the doubly indirect block
        fread(&singly_block_id, 1, 4, img);
        offset += 4;

        //calling readSingly
        bytes_toread = readSingly(img, singly_block_id, bytes_toread, block_size);
    }
    
    return bytes_toread;
}

int readTriply(FILE *img, int triply_block_id, int block_size, int bytes_toread){
    //for the triply indirect blocks
    

    int doubly_block_id;
    int offset = triply_block_id * block_size;

    while(offset < block_size && bytes_toread > 0){
        //moving the file_pointer to offset 
        fseek(img, offset, SEEK_SET);
        //reading a doubly indirect block id from the triply indirect block
        fread(&doubly_block_id, 1, 4, img);
        offset += 4;

        //calling readDoubly
        bytes_toread = readDoubly(img, doubly_block_id, bytes_toread, block_size);
    }

    return bytes_toread;
}