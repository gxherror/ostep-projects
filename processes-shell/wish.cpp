#include <iostream>
#include <fstream>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <sys/wait.h>
#include <string.h>
#include <array>
#define MAXARGS 10
#define MAXPATH 10
#define ERR "An error has occurred\n"
using namespace std;

string builtInCmd[]={"exit","cd","path"};
string externalCmdFindPath="";
int parseCmd(char *buf, string * args);
void parseLine(string line);
void init();
void runBuiltInCmd(string* args);
void runCmd(string * args,bool isLast);

int find(string array[],string value,int length);

int main(int argc, char* argv[])
{
    init();
    if (argc!=1&&argv[1]!=""){
      if (argc!=2){
        fprintf(stderr,ERR);
        exit(EXIT_FAILURE);
      }
        ifstream infile; 
        if (access(argv[1],04)!=-1) {
          //assert(infile.is_open());   
          infile.open(argv[1],ios::in);
        }else{
          fprintf(stderr, ERR);
          exit(1);
        }    
        string line;
        while(getline(infile,line))
        {
          parseLine(line);
        }
        infile.close();
        exit(0);   
    }
    string line;
    while(getline(cin,line)){
      parseLine(line);
    }
    return 0;
    
}

int parseCmd(char *buf, string * args)
{
  /* Points to first space delimiter */
  char *delim;         
  int argc;          
  //buf[strlen(buf)] =' '; 
  /* Ignore leading spaces */
  while (*buf && (*buf == ' ')) 
    buf++;

  /* Build the argv list */
  argc = 0;
  while ((delim = strchr(buf, ' '))) {
    args[argc] = strndup(buf,delim-buf);
    ++argc;
    buf = delim + 1;
    while (*buf && (*buf == ' ')) 
      buf++;
  }
  delim = strchr(buf, '\0');
  args[argc] = strndup(buf,delim-buf);

  if (argc == 0)  /* Ignore blank line */
    return -1;

  /* Should the job run in the background? */
  //if ((bg = (*argv[argc-1] == '&')) != 0)
  //  argv[--argc] = NULL;
  return 0;
}

void runCmd(string args[],bool isLast){
  int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
      int length=0;
      while (args[length]!="") ++length;
      //printf("hello, I am child (pid:%d)\n", (int) getpid());
      int i=find(args,string(">"),length);
      if (i!=-1){
        if (i==0){
          fprintf(stderr,ERR);
          exit(0);
        }
        //if (access(args[i+1].c_str(), F_OK ) == -1){
        //  fprintf(stderr, ERR);
        //  exit(0);
        //}else{
          close(STDOUT_FILENO);
          if (args[i+1]==""||args[i+2]!=""){
            fprintf(stderr,ERR);
            exit(0);
          }else{
            open(args[i+1].c_str(), O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
            length = length-2;
          }
        //}
      }
      char **myargs=new char*[length];
      for (int i=0;i<length;++i){
        myargs[i]=strdup(args[i].c_str());
      }
      myargs[length]=NULL;
      //char *path = getenv("PATH");
      char* linePtr=strdup(externalCmdFindPath.c_str());
      bool isFind = false;
      char* tmp;
      for (char* path=strsep(&linePtr,":");path!=NULL;path=strsep(&linePtr,":")){
        tmp=strdup(path);
        strcat(tmp,myargs[0]);
        if (access(tmp,X_OK)!=-1) {
          isFind=true; 
          break;
        }
      }
      if (isFind){execv(tmp,myargs);}
      else{
        fprintf(stderr,ERR);
        exit(0);
      }
      //char  pathenv[externalCmdFindPath.length() + sizeof("PATH=")];
      //sprintf(pathenv, "PATH=%s", externalCmdFindPath.c_str());
      //char *env[]={pathenv,NULL};
      //TODO:use execv
      //execvpe(myargs[0], myargs,env); 
      
    } else {
        // parent goes down this path (main)
        // wait children to exit
        //int status=0;
        if (isLast){
          while (wait(NULL)!=-1){} 
          //printf("hello, I am parent of %d (pid:%d)\n",rc, (int) getpid());
        }
    }
}

void init(){
    externalCmdFindPath = externalCmdFindPath.append("/bin/:");
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

void parseLine(string line){
  if (line=="") return;
  char* linePtr=strdup(line.c_str());
  int i=0;
  bool isLast = false;
  for (char* cmd=strsep(&linePtr,"&");cmd!=NULL;cmd=strsep(&linePtr,"&"),i++){
    if (linePtr==NULL) isLast=true;
    int j=0;
    string args[MAXARGS];
    parseCmd(cmd,args);
    int length = sizeof(builtInCmd)/(sizeof(builtInCmd[0]));
    int i = find(builtInCmd,args[0],length);
    if(i!=-1){
      runBuiltInCmd(args);
    }else
    {
      runCmd(args,isLast);
    }  
  }
}

//TODO:exit, cd, and path
void runBuiltInCmd(string args[]){
  int length=0;
  while (args[length]!="") ++length;
  if (args[0]=="exit"){
    if (length!=1){
      fprintf(stderr,ERR);
    }
    /*
    sigset_t mask, prev_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    sigprocmask(SIG_BLOCK, &mask, &prev_mask);
    killpg(0,SIGTERM);
    */
    exit(0);
  }else if (args[0]=="cd")
  {
    if (args[1]!=""&&length==2){
      if (chdir(args[1].c_str())==-1){
        fprintf(stderr, ERR);
        exit(0);
      };
    } else{
      fprintf(stderr, ERR);
      exit(0);
    }
  }else if (args[0]=="path")
  {
    externalCmdFindPath.clear();
    int i=1;
    while(args[i]!=""){
      if (args[i].back()!='/'){args[i].append("/");}
      externalCmdFindPath = externalCmdFindPath.append(args[i]);
      externalCmdFindPath = externalCmdFindPath.append(":");
      ++i;
    }
  }else{
    return;
  }
  
  
}





//https://zhuanlan.zhihu.com/p/360923356