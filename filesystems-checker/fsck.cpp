#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h> 
#include <stdio.h>
#include <iostream>
#include "fs.h"
using namespace std;
void* init(int argc , char* argv[]);

struct stat statbuf;
struct superblock *sb;

int main(int argc ,char * argv[],char* env[]){
    char* ptr =(char *)init(argc,argv);
    //!! 
    sb =(superblock *)(ptr+512);
    cout<<sb->size;
    /*
    ssize_t n = write(STDOUT_FILENO,ptr+512,512);
    if(n != 512){
        printf("Write failed");
    }
    */
    return 0;   
}

void* init(int argc , char* argv[]){
            if(argc < 2){
        printf("File path not mentioned\n");
        exit(1);
    }
   
    const char *fullFileName = argv[1];
    int fd = open(fullFileName, O_RDWR);
    if(fd < 0){
        printf("\n\"%s \" could not open\n",fullFileName);
        exit(1);
    }
    int err = fstat(fd, &statbuf);
    if(err < 0){
        printf("\n\"%s \" could not open\n",fullFileName);
        exit(1);
    }

    void *ptr = mmap(NULL,statbuf.st_size,PROT_READ|PROT_WRITE,MAP_PRIVATE,fd,0);
    if(ptr == MAP_FAILED){
        char *msg = strerror(errno);
        printf("Mapping Failed,error msg: %s\n",msg);
        
        exit(1);
    }
    close(fd);
    return ptr;
}