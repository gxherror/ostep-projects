#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <wait.h>
#include <string.h>
#define MAXARGS 10
#define MAXPATH 10
using namespace std;
string builtInCmd[]={"ls","show","add"};
string externalCmdFindPath[MAXPATH];
int parseLine(char *buf, string * args);


template<typename T> 
int find(T array[],T value){
  int i=0;
  while (value!=array[i]&&i<sizeof(array))
    {
      ++i;
    }
    if (i!=sizeof(builtInCmd)){
        return i;
    }else{
        return -1;
    }
}

void init();

void runBuiltInCmd(string* args);

void runCmd(string * args);

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
      if(find(builtInCmd,args[0])){
        //runBuiltInCmd(args);
      }else
      {
        //runCmd(args);
      }
      
      
    }
    return 0;
    
}

int parseLine(char *buf, string * args)
{
  char *delim;         /* Points to first space delimiter */
  int argc;            /* Number of args */

  buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
  while (*buf && (*buf == ' ')) /* Ignore leading spaces */
    buf++;

  /* Build the argv list */
  argc = 0;
  while ((delim = strchr(buf, ' '))) {
    args[argc] = strndup(buf,delim-buf);
    ++argc;
    *delim = '\0';
    buf = delim + 1;
    while (*buf && (*buf == ' ')) /* Ignore spaces */
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

void runCmd(string * args){
  int rc = fork();
        if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) { 
        // child (new process)
        printf("hello, I am child (pid:%d)\n", (int) getpid());
        char **myargs=new char*[sizeof(args)];
        execvp(myargs[0], myargs); 
        printf("this shouldn't print out");
    } else {
        // parent goes down this path (main)
        // wait children to exit
        int wc=wait(NULL);
        printf("hello, I am parent of %d (pid:%d)\n",
        rc, (int) getpid());
    }
}


void init(){
  for (int i=0;i<MAXPATH-1;++i){
    externalCmdFindPath[i]="";
  }
}










//https://zhuanlan.zhihu.com/p/360923356