#ifndef READDIRECTORIES_H
#define READDIRECTORIES_H

int readDirectories(char mode, char *filename);
int readSubDirectories(FILE *img, struct superblock sb, struct group_descriptor gdt[], struct inode file_inode, int depth, char mode, char *filename);

#endif
