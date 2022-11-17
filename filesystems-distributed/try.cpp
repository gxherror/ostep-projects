#include<iostream>
#include<string.h>
#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "lfs.h"
using namespace std;
int main(){
    int fd = MMAP_init("lfs.img");
    extern checkregion* cr;
}
