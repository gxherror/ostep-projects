#ifndef LFS_h
#define LFS_h
// On-disk file system format.
// Both the kernel and user programs use this header file.



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
int MMAP_init(char* fs_name);

int fsfd;
struct checkregion *cr;
block * freeptr;
block * bptr;
uint free_inode;
int nimaps; //number of imap blocks
int ninodes;    // number of inode blocks
int nblocks;  // number of data blocks


#endif