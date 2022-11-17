#include "csapp.h"
#define NTHREADS 4
#define SBUFSIZE 16

typedef struct {
int *buf;/*Buffer array */
int n;/*Maximum number of slots */
int front;/*buf[(front+1)%n] is first item */
int rear;/*buf[rear%n] is last item */
sem_t mutex;/*Protects accesses to buf */
sem_t slots;/*Counts available slots */
sem_t items;/*Counts available items */
} sbuf_t;

/* Create an empty, bounded, shared FIFO buffer with n slots */
void sbuf_init(sbuf_t *sp, int n)
{
    sp->buf =(int *)Calloc(n, sizeof(int));
    sp->n = n;
    /* Buffer holds max of n items */
    sp->front = sp->rear = 0;
    /* Empty buffer iff front == rear */
    Sem_init(&sp->mutex, 0, 1);
    /* Binary semaphore for locking */
    Sem_init(&sp->slots, 0, n);
    /* Initially, buf has n empty slots */
    Sem_init(&sp->items, 0, 0);
    /* Initially, buf has zero data items */
}

/* Clean up buffer sp */
void sbuf_deinit(sbuf_t *sp)
{
	Free(sp->buf);
}

/* Insert item onto the rear of shared buffer sp */
void sbuf_insert(sbuf_t *sp, int item)
{
    P(&sp->slots);
    /* Wait for available slot */
    P(&sp->mutex);
    /* Lock the buffer */
    sp->buf[(++sp->rear)%(sp->n)] = item;
    /* Insert the item */
    V(&sp->mutex);
    /* Unlock the buffer */
    V(&sp->items);
    /* Announce available item */
}

/* Remove and return the first item from buffer sp */
int sbuf_remove(sbuf_t *sp)
{
    int item;
    P(&sp->items);
    /* Wait for available item */
    P(&sp->mutex);
    /* Lock the buffer */
    item = sp->buf[(++sp->front)%(sp->n)]; /* Remove the item */
    V(&sp->mutex);
    /* Unlock the buffer */
    V(&sp->slots);
    /* Announce available slot */
    return item;
}


void echo_cnt(int connfd);
void *thread(void *vargp);

sbuf_t sbuf; /* Shared buffer of connected descriptors */

int main(int argc, char **argv)
{
    int i, listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    listenfd = Open_listenfd(argv[1]);
    sbuf_init(&sbuf, SBUFSIZE);
    for (i = 0; i < NTHREADS; i++) /* Create worker threads */
    Pthread_create(&tid, NULL, thread, NULL);
    while (1) {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        sbuf_insert(&sbuf, connfd); /* Insert connfd in buffer */
    }
}

void *thread(void *vargp)
{
    Pthread_detach(pthread_self());
    while (1) {
        int connfd = sbuf_remove(&sbuf); /* Remove connfd from buffer */
        echo_cnt(connfd);
        Close(connfd);
    }
}