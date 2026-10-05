#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "printfunc.h"
#include "definitions.h"
#include "readDirectories.h"


//argc is the length of argv, argv is the array of everything passed to ./main in the cli
int main(int argc, char *argv[]){
    if(argv[1][0] == '0') {
        //outputing the data from superblock and GDT
        //input = ./main 0
        printSuper();
    }
    else if(argv[1][0] == '1'){
        //calling readDirectories in print all directories mode
        //we don't need any filename input, so passing an empty string
        //input = ./main 1
        readDirectories(argv[1][0], "");
    }
    else if(argv[1][0] == '2'){
        //calling readDirectories in print file data mode
        //input = ./main 2 file
        readDirectories(argv[1][0], argv[2]);
    }
}
