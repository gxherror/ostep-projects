#include <iostream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <sys/wait.h>
#include <string.h>
#include <array>
using namespace std;

int main(){
    string args[]={"uuu","ooo","ttt"};
    int length=0;
    while (args[length]!="") ++length;
    char **myargs=new char*[length];
    for (int i=0;i<length;++i){

        myargs[i]=strdup(args[i].c_str());
    }
}