#ifndef MY_TAR_H
#define MY_TAR_H
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <endian.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include <time.h>
#define OCTAL_BASE 8
#define DEC_BASE 10
#define BLOCKSIZE 512
typedef struct posix_header
{                              /* byte offset */
  char name[100];               /*   0 */           //
  char mode[8];                 /* 100 */           //
  char uid[8];                  /* 108 */           //
  char gid[8];                  /* 116 */           //
  char size[12];                /* 124 */           //
  char mtime[12];               /* 136 */           //
  char chksum[8];               /* 148 */
  char typeflag;                /* 156 */           //
  char linkname[100];           /* 157 */
  char magic[6];                /* 257 */
  char version[2];              /* 263 */
  char uname[32];               /* 265 */
  char gname[32];               /* 297 */
  char devmajor[8];             /* 329 */
  char devminor[8];             /* 337 */
  char prefix[155];             /* 345 */
                                /* 500 */
} tar_header;

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"           /* 00 and no null */
#define TVERSLEN 2
/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */
#ifndef MAJOR
#define MAJOR(dev) (((dev)>>8)&0xff)
#define MINOR(dev) ((dev)&0xff)
#endif
typedef struct options {
    bool create;
    bool extract;
    bool append;
    bool list;
    bool update;
    char *archive_name;
    struct args *args;
} tar_opts;

typedef struct args {
    char *name;
    struct args *next;
} arguments;

typedef struct entry {
    tar_header *header;
    char *content;
    struct entry *next;
} entries;

int open_archive_file(tar_opts *options);

tar_opts *get_options(int, char **);
void delete_options(tar_opts *);

void add_to_list(arguments **, char *);
bool is_same(char *one, char *two);
//arguments = init_argument();
void delete_argument_list(arguments **);

int create_archive(int fd_archive, tar_opts *options);

long get_checksum (tar_header *header);
long char_to_long(char *arg);

#endif

// javohir_tojiboyev
