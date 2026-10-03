#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//defining the superblock structure, according to the nongnu docs
//https://www.nongnu.org/ext2-doc/ext2.pdf

struct superblock {
    uint32_t s_inodes_count;            //total number of inodes, both used and free
    uint32_t s_blocks_count;            //total number of blocks in the system, used, free or reserver
    uint32_t s_reserved_blocks_count;   //total number of blocks reserved for the usage of superuser
    uint32_t s_free_blocks_count;       
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_log_block_size;
    uint32_t s_log_frag_size;
    uint32_t s_blocks_per_group;
    uint32_t s_frags_per_group;
    uint32_t s_inodes_per_group;
    uint32_t s_mtime;
    uint32_t s_wtime;
    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic;
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    uint32_t s_lastcheck;
    uint32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
    uint16_t s_def_resuid;
    uint16_t s_def_resgid;
};

//defining the block group descriptor table (GDT), according to teh nongnu docs
struct group_descriptor {
    uint32_t bg_block_bitmap;
    uint32_t bg_inode_bitmap;
    uint32_t bg_inode_table;
    uint16_t bg_free_blocks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;
    uint8_t  bg_reserved[12];
};


int main(){
    //to open the file, assuming the diskimage is in the same directory
    FILE* img;
    img = fopen("disk-backpup.img", "rb");

    //setting the file pointer to the first bit of superblock
    fseek(img, 1024, SEEK_SET);

    //reading the suberblock (SB)
    struct superblock sb;
    fread(&sb, 1, sizeof(struct superblock), img);        //the pointer to the struct, the size of each element to be read, the number of each element to be read, the pointer to the file
    
        
    //printing out all the superblock fields
    printf("Inodes count - %i\n", sb.s_inodes_count);
    printf("Blocks count - %i\n", sb.s_blocks_count);
    printf("Reserved blocks count - %i\n", sb.s_reserved_blocks_count);
    printf("Free blocks count - %i\n", sb.s_free_blocks_count);
    printf("Free inodes count - %i\n", sb.s_free_inodes_count);
    printf("First data block - %i\n", sb.s_first_data_block);
    printf("Block size (bytes) - %i\n", 1024 << sb.s_log_block_size);
    printf("Fragment size (bytes) - %i\n", 1024 << sb.s_log_frag_size);
    printf("Blocks per group - %i\n", sb.s_blocks_per_group);
    printf("Fragments per group - %i\n", sb.s_frags_per_group);
    printf("Inodes per group - %i\n", sb.s_inodes_per_group);
    printf("The last time the filesystem was mounted - %i\n", sb.s_mtime);
    printf("The last time the filesystem was modified - %i\n", sb.s_wtime);
    printf("Number of times the filesystem has been mounted since last full verification - %i\n", sb.s_mnt_count);
    printf("Maximum number of times the filesystem can be mounted before a full verification - %i\n", sb.s_max_mnt_count);
    printf("Magic number - %i\n", sb.s_magic);
    printf("state of the filesystem - %i\n", sb.s_state);
    printf("error state of the filesystem - %i\n", sb.s_errors);
    printf("Minor revision level - %i\n", sb.s_minor_rev_level);
    printf("The last time the filesystem was checked - %i\n", sb.s_lastcheck);
    printf("Maximum time allowed between system checks - %i\n", sb.s_checkinterval);
    printf("Identifier of the OS that created the filesystem - %i\n", sb.s_creator_os);
    printf("Revision level - %i\n", sb.s_rev_level);
    printf("Default user ID - %i\n", sb.s_def_resuid);
    printf("Default group ID - %i\n", sb.s_def_resgid);
    
    
    printf("\n");
    
    
    int num_groups = (sb.s_blocks_count + sb.s_blocks_per_group - 1) / sb.s_blocks_per_group; //gives the number of groups, rounded up
    struct group_descriptor gdt[num_groups];
    
    //reading the GDT
    for(int i  = 0; i < num_groups; i++) {
        //setting the file pointer to the first bit of the GDT
        //the GDT is stored in the first block right after the superblock
        //sb.s_first_data_block gives the block number of the superblock, 1024 << sb.s_log_block_sze gives the size of a block
        fseek(img, (sb.s_first_data_block + 1) * (1024 << sb.s_log_block_size) + sizeof(struct group_descriptor) * i, SEEK_SET);
        
        fread(&gdt[i], 1, sizeof(struct group_descriptor), img);
    }

    //printing all the group descriptor fields for every group block
    for(int i  = 0; i < num_groups; i++) {
        printf("Block group %i\n", i+1);
        printf("Block ID of block bitmap - %i\n", gdt[i].bg_block_bitmap);
        printf("Block ID of inode bitmap - %i\n", gdt[i].bg_inode_bitmap);
        printf("Block ID of inode table - %i\n", gdt[i].bg_inode_table);
        printf("Number of free blocks in group - %i\n", gdt[i].bg_free_blocks_count);
        printf("Number of free inodes in group - %i\n", gdt[i].bg_free_inodes_count);
        printf("Number of directories in group - %i\n", gdt[i].bg_used_dirs_count);
        printf("\n");
    }
}
