#ifndef READFILES_H
#define READFILES_H

int readFiles(FILE *img, struct superblock sb, struct group_descriptor gdt[], struct inode file_inode);
int readBlock(FILE *img, int block_id, int block_size, int bytes_toread);
int readSingly(FILE *img, int singly_block_id, int block_size, int bytes_toread);
int readDoubly(FILE *img, int doubly_block_id, int block_size, int bytes_toread);
int readTriply(FILE *img, int triply_block_id, int block_size, int bytes_toread);

#endif
