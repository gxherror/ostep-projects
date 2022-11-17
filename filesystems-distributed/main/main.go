package main

/*
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/mman.h>
#include <stddef.h>

#define MAXBLOCK 1000
#define ROOTINO 0  // root i-number
#define BSIZE 512  // block size
#define MAXINODE 4096 //max inode number
#define CRMETA 40

#define IMAPMETA 40
#define IPMP ((BSIZE-IMAPMETA) / sizeof(inode*)) //inodes per imap block
#define MAXIMAP (MAXINODE/IPMP) //max imap number
#define IMBLOCK(i) (i/IPMP) // imap block containing inode i

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 28

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

// Disk layout:
// [ check region | data blocks | inode blocks | imap block ]

// On-disk inode structure
#define NDIRECT 14
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)
#define TYPE_FILE 0
#define TYPE_DIRECTORY 1

typedef struct block
{
  uchar reserverd[BSIZE];
}block;


typedef struct inode {
  short type;           // File type
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  block* data_addrs[NDIRECT+1];   // Data block addresses
}inode;


typedef struct imap
{
    uchar reserverd[IMAPMETA];
    inode* inode_ptr[IPMP];
}imap;

// super block describes the disk layout:
typedef struct checkregion {
  uint nblocks;      // number of data blocks
  uint ninodes;      // number of inode blocks
  uint nimaps;       // number of imaps blocks
  uint total_blocks;
  uint free_inode;
  block *freeptr;
  uchar reserved[CRMETA-4*sizeof(uint)];
  imap* imap_ptr[MAXIMAP]; //imap block address
}checkregion;

typedef struct dirent {
    char name[28];  // up to 28 bytes of name in directory (including \0)
    int  inum;      // inode number of entry (-1 means entry not used)
}dirent;

void balloc(int);
void wsect(block *buf,int length);
void rsect(block * dstptr, void *buf,int length);
void iappend(uint inum, void *p, int n);
void LFS_test();
int LFS_Init();
int LFS_Lookup(char *name);
int LFS_Stat(int inum, inode *m);
void LFS_Write(block* data_ptr,int length,inode* inode_ptr,uint inode_num);
void LFS_FileWrite(char * name);
void LFS_FileRead(char * name,block *block_ptr);
void LFS_Read(uint inode_num,block* data_ptr,inode* inode_ptr);
int LFS_Creat(int pinum, int type, char *name);
int LFS_Unlink(int pinum, char *name);
int LFS_Shutdown();
void IMAP_Write(imap* imap);
void DATA_Write(block* data,inode* inode,int length);
void INODE_Write(inode* inode);

int MMAP_init();

int fsfd;
struct checkregion *cr;
block * freeptr;
block * bptr;
uint free_inode;
int nimaps; //number of imap blocks
int ninodes;    // number of inode blocks2
int nblocks;  // number of data blocks

//mmap init
int  MMAP_init(){
    const char *fullFileName ="lfs.img";
    struct stat statbuf;
    fsfd = open(fullFileName, O_RDWR);
    if(fsfd < 0){
        printf("\n\"%s \" could not open\n",fullFileName);
        exit(1);
    }
    int err = fstat(fsfd, &statbuf);
    if(err < 0){
        printf("\n\"%s \" could not open\n",fullFileName);
        exit(1);
    }

    void *ptr = mmap(NULL,MAXBLOCK*BSIZE,PROT_READ|PROT_WRITE,MAP_PRIVATE,fsfd,0);
    if(ptr == MAP_FAILED){
        printf("Mapping Failed,error msg: \n");

        exit(1);
    }
    cr = (checkregion*)ptr;
    free_inode = cr->free_inode;
    freeptr = cr->freeptr;
    bptr = (block *)ptr;
    //freeptr = bptr;
    return fsfd;
}

int LFS_Init(){
  cr =(checkregion*)freeptr;
  cr->nblocks = 0;
  cr->nimaps = 0;
  cr->ninodes = 0;
  cr->total_blocks = 0;
  cr->free_inode = 0;
  cr->freeptr =NULL;
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
  memcpy(freeptr,buf,length);
  ++freeptr;
};

void rsect(block * dstptr, void *buf,int length)
{
  memcpy(buf,dstptr,length);

}

void LFS_FileWrite(char * name){
  access(name,7);
  int fd = open (name,O_RDWR);
  struct stat statbuf;
  int err = fstat(fd, &statbuf);
  int block_num = statbuf.st_size / BSIZE +1;
  //block* block_ptr =(block*)malloc(block_num*BSIZE);
  block block_ptr;
  memset(&block_ptr,0x0,statbuf.st_size);
  read(fd,&block_ptr,statbuf.st_size);

  inode test_inode;
  int inode_num = free_inode ;
  ++free_inode;
  memset(&test_inode,0x0,sizeof(inode));
  test_inode.size = statbuf.st_size;
  test_inode.nlink = 1;
  test_inode.type = TYPE_FILE;
  LFS_Write(&block_ptr,statbuf.st_size,&test_inode,inode_num);

  block tmp_data;
  memset(&tmp_data,0x0,BSIZE);
  dirent * root_dirnet;
  inode tmp_inode;
  memset(&tmp_inode,0x0,sizeof(inode));
  LFS_Read(ROOTINO,&tmp_data,&tmp_inode);
  root_dirnet = (dirent *)&tmp_data;
  root_dirnet[3].inum = inode_num;
  strcpy(root_dirnet[3].name ,name);
  LFS_Write(&tmp_data,NDIRECT*sizeof(dirent),&tmp_inode,ROOTINO);
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
    int length=0;
    for (;imap_ptr_old->inode_ptr[length]!=NULL;++length);
    //!!with bug
    memcpy(tmp_imap.inode_ptr,imap_ptr_old->inode_ptr,length*sizeof(inode*));
  }
  tmp_imap.inode_ptr[nino_offset]=(inode *)(freeptr-1);
  IMAP_Write(&tmp_imap);
  cr->imap_ptr[nimap]=(imap*)(freeptr-1);
}

void IMAP_Write(imap* imap_ptr){
  wsect((block*)imap_ptr,sizeof(imap));
  ++cr->nimaps;
}

void DATA_Write(block* data_ptr,inode *inode_ptr,int length){
  int block_num = length / BSIZE ;
  if (block_num > NDIRECT) exit(1);
  int n=length;
  int i=0;
  while (n>0)
  {
    int n1 = (n<BSIZE)?n:BSIZE;
    inode_ptr->data_addrs[i]=freeptr;
    ++cr->nblocks;
    ++i;
    wsect(data_ptr,n1);
    n = n -n1;
  }
}

void INODE_Write(inode* inode_ptr){
  wsect((block*)inode_ptr,sizeof(inode));
  ++cr->ninodes;
}

void LFS_FileRead(char * name,block *block_ptr){
  memset(block_ptr,0x0,BSIZE);
  int inode_num = LFS_Lookup(name);
  inode lookup_inode;
  memset(&lookup_inode,0x0,sizeof(inode));
  LFS_Read(inode_num,block_ptr,&lookup_inode);

}

void LFS_Read(uint inode_num,block* data_ptr,inode* inode_ptr){
  uint nimap = IMBLOCK(inode_num);
  uint ino_offset = inode_num % IPMP;
  imap* imap_ptr=cr->imap_ptr[nimap];
  if (imap_ptr!=NULL){
    //*inode_ptr = imap_ptr->inode_ptr[ino_offset];
    //*data_ptr = *(inode_ptr)->data_addrs[0];
    *inode_ptr = *(imap_ptr->inode_ptr[ino_offset]);
    *data_ptr = *(inode_ptr->data_addrs[0]);
  }
}

int LFS_Lookup(char* name){
  assert(strchr(name, '/') == name);
  ++name;
  block tmp_data;
  memset(&tmp_data,0x0,BSIZE);
  dirent * root_dirnet;
  inode tmp_inode;
  memset(&tmp_inode,0x0,sizeof(inode));
  LFS_Read(ROOTINO,&tmp_data,&tmp_inode);
  root_dirnet = (dirent *)&tmp_data;
  for (;strcmp(root_dirnet->name,name)!=0 && root_dirnet->inum!=-1;++root_dirnet){};
  return root_dirnet->inum;
}

void LFS_test(){

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

  block tmp_data;
  memset(&tmp_data,0x0,BSIZE);
  dirent * root_dirnet;
  inode tmp_inode;
  memset(&tmp_inode,0x0,sizeof(inode));
  LFS_Read(ROOTINO,&tmp_data,&tmp_inode);
  root_dirnet = (dirent *)&tmp_data;
  root_dirnet[2].inum = test_inode_num;
  strcpy(root_dirnet[2].name ,"foo.txt");
  LFS_Write(&tmp_data,NDIRECT*sizeof(dirent),&tmp_inode,ROOTINO);

  block lookup_data;
  memset(&lookup_data,0x0,BSIZE);
  inode lookup_inode;
  memset(&lookup_inode,0x0,sizeof(inode));
  int inode_num = LFS_Lookup("/foo.txt");
  LFS_Read(inode_num,&tmp_data,&tmp_inode);
  write(STDOUT_FILENO,&tmp_data,BSIZE);
}
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

int LFS_Shutdown(){
  cr->total_blocks = cr->nblocks +cr->ninodes +cr->nimaps +1;
  cr->free_inode = free_inode;
  cr->freeptr = freeptr;
  //memcpy(bptr,(char*)cr,BSIZE);
  write(fsfd,bptr,MAXBLOCK*BSIZE);
  munmap(bptr,BSIZE);
  close(fsfd);
}

*/
import "C"


func main() {
	C.MMAP_init()
}
