#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char *argv[], char *envp[]){
    pid_t getpid(void);
    pid_t getppid(void);
    //Returns: 0 to child, PID of child to parent, −1 on error
    //Shared files ,E:STDOUT
    pid_t fork(void);
    //Does not return if OK; returns −1 on error
    int execve(const char *filename, const char *argv[],const char *envp[]);
    pid_t waitpid(pid_t pid, int *statusp, int options);
    //Calling wait(&status) is equivalent to calling waitpid(-1, &status, 0).
    pid_t wait(int *statusp);

    unsigned int sleep(unsigned int secs);
    int pause(void);

    char *getenv(const char *name);
    int setenv(const char *name, const char *newvalue, int overwrite);
    void unsetenv(const char *name);
    
    void exit(int status);
}