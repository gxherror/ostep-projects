 /* Safe SIGINT handler */
#include "csapp.h"

void handler(int sig)
{
    int olderrno = errno;
    while (waitpid(-1, NULL,WNOHANG) > 0) {
        Sio_puts("Handler reaped child\n");
    }
    /*
    if (errno != ECHILD)
    Sio_error("waitpid error");
    Sleep(1);
    */
    errno = olderrno;
};

long counter = 2;

void handler1(int sig)
{
    sigset_t mask, prev_mask;
    Sigfillset(&mask);
    Sigprocmask(SIG_BLOCK, &mask, &prev_mask);
    /* Block sigs */
    Sio_putl(--counter);
    Sigprocmask(SIG_SETMASK, &prev_mask, NULL); /* Restore sigs */
    _exit(0);
}
int main()
{
    int i, n;
    char buf[MAXBUF];
    pid_t pid;
    sigset_t mask, prev_mask;
    printf("%ld", counter);
    fflush(stdout);
    signal(SIGUSR1, handler1);
    if ((pid = Fork()) == 0) {
        while(1) {};
    }
    Kill(pid, SIGUSR1);
    Waitpid(-1, NULL, 0);
    Sigfillset(&mask);
    Sigprocmask(SIG_BLOCK, &mask, &prev_mask); /* Block sigs */
    printf("%ld", ++counter);
    Sigprocmask(SIG_SETMASK, &prev_mask, NULL); /* Restore sigs */
    exit(0);
    /*
    if (signal(SIGCHLD, handler) == SIG_ERR)
    	unix_error("signal error");
    */    
    /* Parent creates children */

    /*
    for (i = 0; i < 3; i++) {
        if (fork() == 0) {
            printf("Hello from child %d\n", (int)getpid());
            sleep(i);
            exit(0);
        }
    }
    */
    /* Parent waits for terminal input and then processes it */

    /*
    printf("Parent processing input\n");

    while (1)
    {
        if ((n = read(STDIN_FILENO, buf, sizeof(buf))) < 0)
    	    unix_error("read");
        else{
            printf("receive : %s" ,buf);
        }
    }
    */


    exit(0);
}