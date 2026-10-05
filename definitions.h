
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

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

//defining the inode structure
struct inode {
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks;
    uint32_t i_flags;
    uint32_t i_osd1;
    uint32_t i_block[15];
    
    uint32_t i_generation;
    uint32_t i_file_acl;
    uint32_t i_dir_acl;
    uint32_t i_faddr;
    uint8_t  i_osd2[12];
};

//defining the directory inode structure

//pragma forces the compiler to store data with no padding bytes
//since i'm reading the first 8 bits, then i'm reading name, it might fill name as well with garbage bytes
#pragma pack(push, 1)
struct directory {
    uint32_t inode;
    uint16_t rec_len;
    uint8_t name_len;
    uint8_t file_type;
    char name[256];
};
//stops forcing no padding bytes
#pragma pack(pop)

#endif
