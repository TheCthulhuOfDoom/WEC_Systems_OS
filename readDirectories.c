#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include "definitions.h"
#include "readDirectories.h"

int readDirectories(){

    //obtaining the superblock and GDT structures
    FILE* img;
    img = fopen("disk-backpup.img", "rb");

    fseek(img, 1024, SEEK_SET);

    struct superblock sb;
    fread(&sb, 1, sizeof(struct superblock), img);

    int num_groups = (sb.s_blocks_count + sb.s_blocks_per_group - 1) / sb.s_blocks_per_group;
    struct group_descriptor gdt[num_groups];
    
    for(int i  = 0; i < num_groups; i++) {
        fseek(img, (sb.s_first_data_block + 1) * (1024 << sb.s_log_block_size) + sizeof(struct group_descriptor) * i, SEEK_SET);
        fread(&gdt[i], 1, sizeof(struct group_descriptor), img);
    }




    //finding the byte offset of the inode of the root directory
    //offset = (block id of the first block of the inode table * size of a block) + (inode id - 1) * size of an inode
    //the size of the inode in the diskimage is 256 here
    
    //we only need to access the first block group since that's where the root directory is located, all files are connected to root directory
    int offset = (gdt[0].bg_inode_table * (1024 << sb.s_log_block_size)) + (1 * 256);


    //moving the file pointer to the root inode
    fseek(img, offset, SEEK_SET);

    //saving the root inode to a struct variable
    struct inode root_inode;
    fread(&root_inode, 1, sizeof(struct inode), img);

    readSubDirectories(img, sb, gdt, root_inode, 0);
}


int readSubDirectories(FILE *img, struct superblock sb, struct group_descriptor gdt[], struct inode file_inode, int depth){


    

    //moving the file pointer to the first block stored in the inode
    // fseek(img, file_inode.i_block[0] * (1024 << sb.s_log_block_size), SEEK_SET);

    //reading the first folder "." to the struct
    // struct directory file_dir;
    // fread(&file_dir, 1, 12, img);


    

    int i = 0;
    int offset, increase, toskip;

    //definition of the variable in which we're storing the formatted name
    // char name[12];
    struct directory file_dir;
    // char *names;

    //outer while loop loops through all the datablocks stored in the inode struct
    while(i < 12){
        if(file_inode.i_block[i] == 0){
            i++;
            continue;
        }
        offset = file_inode.i_block[i] * (1024 << sb.s_log_block_size);
        increase = 0;

        //moving the file pointer to the block we want to access
        fseek(img, offset + increase, SEEK_SET); 
        //reading the contents of that block to file_dir
        fread(&file_dir, 1, 8, img);
        fread(&file_dir.name, 1, file_dir.name_len, img);
        //increase tracks how many bytes we have read in the current block
        increase = file_dir.rec_len;

        //output of snprintf is instead written to names
        // snprintf(names, sizeof(char) * file_dir.name_len + 1, "%.*s", file_dir.name_len, file_dir.name);
        // printf("%s %c\n", names, names[file_dir.name_len - 1]);
        // free(names);
        for(int i = 0; i < depth; i++) printf("\t");
        printf("%.*s\n", file_dir.name_len, file_dir.name);


        //the file pointer moves forward automatically when using fread, so we don't need to use fseek from this point onwards

        //inner while loop loops within the datablock
        while(increase < (1024 << sb.s_log_block_size)){
            fseek(img, offset + increase, SEEK_SET); 
            fread(&file_dir, 1, 8, img);

            //reads the 
            fread(&file_dir.name, 1, file_dir.name_len, img);


            increase += file_dir.rec_len;

            //printing the name of the directory
            // names = malloc(file_dir.name_len + 1);
            // snprintf(names, sizeof(char) * file_dir.name_len + 1, "%.*s", file_dir.name_len, file_dir.name);
            for(int i = 0; i < depth; i++) printf("\t");
            printf("%.*s\n", file_dir.name_len, file_dir.name);
            
            
            //to check whether to skip . and .. directories
            toskip = ((file_dir.name_len == 1 && file_dir.name[0] == '.') || (file_dir.name_len == 2 && file_dir.name[0] == '.' && file_dir.name[1] == '.'));
            if(!toskip && file_dir.file_type == 2){
                //we will pass the child inode back to the function to create the recursion

                //block offset of the inode of the child directory
                //index = 
                int index = (file_dir.inode - 1) / sb.s_inodes_per_group;
                //offset = 
                int offset = (gdt[index].bg_inode_table * (1024 << sb.s_log_block_size)) + ((file_dir.inode - 1) % sb.s_inodes_per_group * 256);
                struct inode child_file_inode;

                fseek(img, offset, SEEK_SET);
                fread(&child_file_inode, 1, sizeof(struct inode), img);
                
                readSubDirectories(img, sb, gdt, child_file_inode, depth + 1);
            }
            // free(names);

        }

        printf("\n");
        //forgot what this bit of code does
        i++;

    }
}
