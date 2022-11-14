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

struct block
{
  uchar reserverd[BSIZE];
};

// On-disk inode structure
#define NDIRECT 14
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)
#define TYPE_FILE 0
#define TYPE_DIRECTORY 1
struct inode {
  short type;           // File type
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  block* data_addrs[NDIRECT+1];   // Data block addresses
};


struct imap
{
    uchar reserverd[IMAPMETA];
    inode* inode_ptr[IPMP];
};

// super block describes the disk layout:
struct checkregion {
  uint nblocks;      // number of data blocks
  uint ninodes;      // number of inode blocks
  uint nimaps;       // number of imaps blocks  
  uint total_blocks;
  uint free_inode;
  block *freeptr;
  uchar reserved[CRMETA-4*sizeof(uint)];
  imap* imap_ptr[MAXIMAP]; //imap block address
};

struct dirent {
    char name[28];  // up to 28 bytes of name in directory (including \0)
    int  inum;      // inode number of entry (-1 means entry not used)
};

#endif