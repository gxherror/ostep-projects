#include "lfs.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <sys/mman.h>

/*
#include "types.h"
#include "fs.h"
#include "stat.h"
#include "param.h"
*/
using namespace std;
#ifndef static_assert
#define static_assert(a, b) do { switch (0) case 0: case (a): ; } while (0)
#endif

// Disk layout:
// [ check region | data blocks | inode blocks | imap block ]

int nimaps; //number of imap blocks
int ninodes;    // number of inode blocks
int nblocks;  // number of data blocks

int fsfd;
struct checkregion *cr;
char zeroes[BSIZE];
block * freeptr=NULL; 
uint free_inode=0;

void balloc(int);
void wsect(block *buf,int length);
void winode(uint, struct dinode*);
void rinode(uint inum, struct dinode *ip);
void rsect(block * dstptr, void *buf,int length);
uint ialloc(ushort type);
void iappend(uint inum, void *p, int n);

// convert to intel byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar*)&y;
  a[0] = x;
  a[1] = x >> 8;
  return y;
}

uint
xint(uint x)
{
  uint y;
  uchar *a = (uchar*)&y;
  a[0] = x;
  a[1] = x >> 8;
  a[2] = x >> 16;
  a[3] = x >> 24;
  return y;
}

int LFS_Init();
int LFS_Lookup(char *name);
int LFS_Stat(int inum, dinode *m);
void LFS_Write(block* data_ptr,int length,inode* inode_ptr,uint inode_num);
void LFS_Read(uint inode_num,block* data_ptr,inode* inode_ptr);
int LFS_Creat(int pinum, int type, char *name);
int LFS_Unlink(int pinum, char *name);
int LFS_Shutdown();
void IMAP_Write(imap* imap);
void DATA_Write(block* data,inode* inode,int length);
void INODE_Write(inode* inode);
void* MMAP_init(int argc , char* argv[]);

int LFS_Init(){

  cr =(checkregion*)freeptr;
  cr->nblocks = 0;
  cr->nimaps = 0;
  cr->ninodes = 0;
  for (int i=0;i<MAXIMAP;++i){
    cr->imap_ptr[i] = NULL ;
  }
  ++freeptr;

  block root_block;
  memset(&root_block,0x0,NDIRECT*sizeof(dirent));
  dirent *root_dirent = (dirent*)&root_block;
  for (int i=0;i<NDIRECT;++i){
    root_dirent[i].inum = -1;
  }
  strcpy(root_dirent->name,".");
  root_dirent->inum=free_inode;
  ++root_dirent;
  strcpy(root_dirent->name,"..");
  root_dirent->inum=free_inode;

  inode root_inode;
  memset(&root_inode,0x0,sizeof(inode));
  root_inode.size = 2*sizeof(dirent);
  root_inode.nlink = 1;
  root_inode.type = TYPE_DIRECTORY;
  LFS_Write(&root_block,NDIRECT*sizeof(dirent),&root_inode,free_inode);
  ++free_inode;
}

void wsect(block *buf,int length){
  /*
  if(lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE){
    perror("lseek");
    exit(1);
  }
  if(write(fsfd, buf, BSIZE) != BSIZE){
    perror("write");
    exit(1);
  }
  */
  memcpy(freeptr,buf,length);
  ++freeptr;
};

void rsect(block * dstptr, void *buf,int length)
{
  /*
  if(lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE){
    perror("lseek");
    exit(1);
  }
  if(read(fsfd, buf, BSIZE) != BSIZE){
    perror("read");
    exit(1);
  }
  */
  memcpy(buf,dstptr,length);

}

void LFS_Write(block* data_ptr,int length,inode* inode_ptr,uint inode_num){
  uint nimap = IMBLOCK(inode_num);
  uint nino_offset = inode_num % IPMP;
  imap* imap_ptr_old=cr->imap_ptr[nimap];
  //imap don't exist ,make a new imap  
  DATA_Write(data_ptr,inode_ptr,length);
  INODE_Write(inode_ptr);
  imap tmp_imap;
  memset(&tmp_imap,0x0,sizeof(imap));
  if (imap_ptr_old!=NULL){
    memcpy(tmp_imap.inode_ptr,imap_ptr_old->inode_ptr,nino_offset*sizeof(*inode_ptr));
  }
  tmp_imap.inode_ptr[nino_offset]=(inode *)(freeptr-1);
  IMAP_Write(&tmp_imap);
  cr->imap_ptr[nimap]=(imap*)(freeptr-1);
}

void IMAP_Write(imap* imap){
  wsect((block*)imap,sizeof(imap));
}

void DATA_Write(block* data_ptr,inode *inode_ptr,int length){
  int block_num = length / BSIZE ;
  if (block_num > NDIRECT) exit(1);
  int n=length;
  int i=0;
  while (n>0)
  {
    int n1 = min(BSIZE,n);
    inode_ptr->data_addrs[i]=freeptr;
    ++i;
    wsect(data_ptr,n1);
    n = n -n1;
  }
}

void INODE_Write(inode* inode_ptr){
  wsect((block*)inode_ptr,sizeof(inode));
}

void LFS_Read(uint inode_num,block** data_ptr,inode** inode_ptr){
  uint nimap = IMBLOCK(inode_num);
  uint ino_offset = inode_num % IPMP;
  imap* imap_ptr=cr->imap_ptr[nimap];
  if (imap_ptr!=NULL){
    *inode_ptr = imap_ptr->inode_ptr[ino_offset];
    *data_ptr = (*inode_ptr)->data_addrs[0];
  }
}

int
main(int argc, char *argv[])
{
  int i, cc, fd;
  uint rootino, inum, off;
  struct dirent de;
  char buf[BSIZE];

  static_assert(sizeof(int) == 4, "Integers must be 4 bytes!");
  assert((BSIZE % sizeof(struct dirent)) == 0);

  block* bptr =(block *) MMAP_init(argc,argv);
  freeptr = bptr;
  LFS_Init();
  //write /foo

  block test_block;
  uint test_len = strlen("foofoofoo");
  uint test_inode_num = free_inode; 
  ++free_inode;
  memset(&test_block,0x0,test_len);
  memcpy(&test_block,"foofoofoo",test_len);

  inode test_inode;
  memset(&test_inode,0x0,sizeof(inode));
  test_inode.size = test_len;
  test_inode.nlink = 1;
  test_inode.type = TYPE_FILE;
  LFS_Write(&test_block,test_len,&test_inode,test_inode_num);

  block * data_ptr;
  dirent * root_dirnet;
  inode *inode_ptr;
  LFS_Read(ROOTINO,&data_ptr,&inode_ptr);
  root_dirnet = (dirent *)data_ptr;
  root_dirnet[2].inum = test_inode_num;
  strcpy(root_dirnet[2].name ,"foo.txt");
  LFS_Write(data_ptr,NDIRECT*sizeof(dirent),inode_ptr,ROOTINO);

  /*
  for(i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, sizeof(buf));
  memmove(buf, &sb, sizeof(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  assert(rootino == ROOTINO);

  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, sizeof(de));

  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, sizeof(de));

  for(i = 2; i < argc; i++){
    assert(index(argv[i], '/') == 0);

    if((fd = open(argv[i], 0)) < 0){
      perror(argv[i]);
      exit(1);
    }

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    if(argv[i][0] == '_')
      ++argv[i];

    inum = ialloc(T_FILE);

    bzero(&de, sizeof(de));
    de.inum = xshort(inum);
    strncpy(de.name, argv[i], DIRSIZ);
    iappend(rootino, &de, sizeof(de));

    while((cc = read(fd, buf, sizeof(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.size);
  off = ((off/BSIZE) + 1) * BSIZE;
  din.size = xint(off);
  winode(rootino, &din);

  balloc(freeblock);
  */
  exit(0);
}

//mmap init 
void* MMAP_init(int argc , char* argv[]){
    if(argc < 2){
        printf("File path not mentioned\n");
        exit(1);
    }
    const char *fullFileName = argv[1];
    struct stat statbuf;
    int fd = open(fullFileName, O_RDWR);
    if(fd < 0){
        printf("\n\"%s \" could not open\n",fullFileName);
        exit(1);
    }
    int err = fstat(fd, &statbuf);
    if(err < 0){
        printf("\n\"%s \" could not open\n",fullFileName);
        exit(1);
    }

    void *ptr = mmap(NULL,statbuf.st_size,PROT_READ|PROT_WRITE,MAP_PRIVATE,fd,0);
    if(ptr == MAP_FAILED){
        char *msg = strerror(errno);
        printf("Mapping Failed,error msg: %s\n",msg);
        
        exit(1);
    }
    close(fd);
    return ptr;
}

/*
void
wsect(uint sec, void *buf)
{
  if(lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE){
    perror("lseek");
    exit(1);
  }
  if(write(fsfd, buf, BSIZE) != BSIZE){
    perror("write");
    exit(1);
  }
}

void
winode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((struct dinode*)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

void
rinode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((struct dinode*)buf) + (inum % IPB);
  *ip = *dip;
}

void
rsect(uint sec, void *buf)
{
  if(lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE){
    perror("lseek");
    exit(1);
  }
  if(read(fsfd, buf, BSIZE) != BSIZE){
    perror("read");
    exit(1);
  }
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  struct dinode din;

  bzero(&din, sizeof(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  return inum;
}

void
balloc(int used)
{
  uchar buf[BSIZE];
  int i;

  printf("balloc: first %d blocks have been allocated\n", used);
  assert(used < BSIZE*8);
  bzero(buf, BSIZE);
  for(i = 0; i < used; i++){
    buf[i/8] = buf[i/8] | (0x1 << (i%8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

void
iappend(uint inum, void *xp, int n)
{
  char *p = (char*)xp;
  uint fbn, off, n1;
  struct dinode din;
  char buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  while(n > 0){
    fbn = off / BSIZE;
    assert(fbn < MAXFILE);
    if(fbn < NDIRECT){
      if(xint(din.addrs[fbn]) == 0){
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } else {
      if(xint(din.addrs[NDIRECT]) == 0){
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (char*)indirect);
      if(indirect[fbn - NDIRECT] == 0){
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (char*)indirect);
      }
      x = xint(indirect[fbn-NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.size = xint(off);
  winode(inum, &din);
}
*/