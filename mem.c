#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
    int *p = malloc(sizeof(int));
    printf("(%d) memory address of p: %08x\n",
    getpid(), (unsigned) p);
    printf("location of code : %p\n",(void *) main );
    int x=0;
    printf("location of stack : %p\n",(void *) &x );
    *p = 0;
    while (1) {
    sleep(1);
    *p = *p + 1;
    printf("(%d) p: %d\n", getpid(), *p); // a4
    }
    return 0;
}