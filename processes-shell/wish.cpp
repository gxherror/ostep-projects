#include <iostream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <sys/wait.h>
#include <string.h>
#include <array>
#define MAXARGS 10
#define MAXPATH 10
using namespace std;

string builtInCmd[]={"ls","show","add"};
string externalCmdFindPath[MAXPATH];
int parseLine(char *buf, string * args);
void init();
void runBuiltInCmd(string* args);
void runCmd(string * args);

int find(string array[],string value,int length);

int main(int argc, char* argv[])
{
    init();
    if (argc!=1){
      /*
        ifstream infile; 
        if (access(argv[1],04)) infile.open(argv[1],ios::in);  
        //assert(infile.is_open());   
        string s;
        char* ptr;
        int i=0;
        while(getline(infile,s))
        {
            int i=0;
            char * strs = new char[s.length() + 1] ;
	        strcpy(strs, s.c_str()); 
            char* lineTemp=strdup(strs);
            for (char* cmd=strsep(&lineTemp,"&");cmd!=NULL;cmd=strsep(&lineTemp,"&"),i++){
                int j=0;
                string args[MAXARGS];
                char* cmdPtr=cmd;
                //ignore blank head
                while (*cmdPtr && (*cmdPtr == ' ')) 
                    cmdPtr++;
                
            }
            //TODO:concurrency run
        }
        infile.close();   
        */
    }
    string line;
    while(getline(cin,line)){
      string args[MAXARGS];
      char * ptr;
      ptr=strdup(line.c_str());
      parseLine(ptr,args);
      
      int length = sizeof(builtInCmd)/(sizeof(builtInCmd[0]));
      if(find(builtInCmd,args[0],length)){
        //runBuiltInCmd(args);
      }else
      {
        runCmd(args);
      }    
    }
    return 0;
    
}

int parseLine(char *buf, string * args)
{
  /* Points to first space delimiter */
  char *delim;         
  int argc;          

  buf[strlen(buf)] =' '; 
  /* Ignore leading spaces */
  while (*buf && (*buf == ' ')) 
    buf++;

  /* Build the argv list */
  argc = 0;
  while ((delim = strchr(buf, ' '))) {
    *delim = '\0';
    args[argc] = strndup(buf,delim-buf);
    ++argc;
    buf = delim + 1;
    /* Ignore spaces */
    while (*buf && (*buf == ' ')) 
      buf++;
  }
  args[argc] = "";

  if (argc == 0)  /* Ignore blank line */
    return -1;

  /* Should the job run in the background? */
  //if ((bg = (*argv[argc-1] == '&')) != 0)
  //  argv[--argc] = NULL;
  return 0;
}

void runCmd(string args[]){
  int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        int length=0;
        while (args[length]!="") ++length;
        printf("hello, I am child (pid:%d)\n", (int) getpid());
        int i=find(args,string(">"),length);
        if (i!=-1){
          if (access(args[i+1].c_str(), F_OK ) == -1){
            fprintf(stderr, "redirect failed\n");
            exit(0);
          }else{
            close(STDOUT_FILENO);
            open(args[i+1].c_str(), O_WRONLY|O_TRUNC, S_IRWXU);
            length = length-2;
          }
        }
        char **myargs=new char*[length];
        for (int i=0;i<length;++i){
          myargs[i]=strdup(args[i].c_str());
        }
        myargs[length]=NULL;
        char *path = getenv("PATH");
        char  pathenv[strlen(path) + sizeof("PATH=")];
        sprintf(pathenv, "PATH=%s", path);

        char *env[]={path,NULL};
        execvpe(myargs[0], myargs,env); 
    } else {
        // parent goes down this path (main)
        // wait children to exit
        int status=0;
        int wc=wait(&status);
        printf("hello, I am parent of %d (pid:%d)\n",rc, (int) getpid());
    }
}


void init(){
  for (int i=0;i<MAXPATH-1;++i){
    externalCmdFindPath[i]="";
    externalCmdFindPath[0]="/usr/bin/";
  }
}

int find(string array[],string value,int length){
  int i=0;
  while (value!=array[i]&& i<length)
    {
      ++i;
    }
    if (i!=length){
        return i;
    }else{
        return -1;
    }
}








//https://zhuanlan.zhihu.com/p/360923356