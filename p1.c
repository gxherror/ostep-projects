#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
int main(int argc, char *argv[])
{
    printf("hello world (pid:%d)\n", (int) getpid());
    //新建一个进程，拥有自己的地址空间，复制父进程全部内容
    int rc = fork();
        if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) { 
        // child (new process)
        printf("hello, I am child (pid:%d)\n", (int) getpid());
        char *myargs[3];
        myargs[0] = strdup("wc");
        myargs[1] = strdup("p1.c");
        myargs[2] = NULL;
        //对当前进程的内容进行替换
        execvp(myargs[0], myargs); 
        printf("this shouldn't print out");
    } else {
        // parent goes down this path (main)
        // wait children to exit
        int wc=wait(NULL);
        printf("hello, I am parent of %d (pid:%d)\n",
        rc, (int) getpid());
    }
    return 0;
}