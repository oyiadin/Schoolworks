#ifndef G3FS_HEADER
#define G3FS_HEADER

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

extern int fd;

#define report_error(msg, val...) \
  do { \
    puts(msg); \
    retval = (-1, ##val); \
    goto out; \
  } while (0)


#define min(a, b) ((a) < (b) ? (a) : (b))

#define BLOCK_SIZE (4 * 1024)
#define BLK_FREE 0
#define BLK_END 1

typedef uint16_t bid_t;
typedef uint8_t blk_t[BLOCK_SIZE];

#define offset_of(x) (sizeof(blk_t) * x)


typedef struct superblock {
  uint16_t magic;
#define MAGIC_SUPERBLOCK 0x1510u
  int total_size;
  int total_block_num;
  int fat_block_num;
  int fcb_num_per_block;
  int data_start_bid;
  int fat_crc;
} sb_t;
#define sb_check_magic(x) (((sb_t *)x)->magic == MAGIC_SUPERBLOCK)


#define FNAME_LENGTH 9
typedef struct fcb {
  char fname[FNAME_LENGTH];  // 9
  uint32_t size;  // 4
  bid_t bid;      // 2
  uint8_t attrs;  // 1
} fcb_t;
#define check_filename_length(x) (strlen(x) <= FNAME_LENGTH)
#define ISDIR_MASK 0b10u
#define ISEXIST_MASK 0b1u
#define fcb_isdir(fcb) ((fcb)->attrs & ISDIR_MASK)
#define fcb_isexist(fcb) ((fcb)->attrs & ISEXIST_MASK)


typedef struct opened_file {
  bool not_empty;
  fcb_t fcb;
  bid_t at_bid;
  int fcb_id;
  off_t off;
  bool is_fcb_modified;
} of_t;
#define MAX_FD 2
extern of_t ofs[MAX_FD];


typedef struct directory {
  uint16_t magic;
#define MAGIC_DIR 0xD151u
  int item_num;
  bid_t bid;
  bid_t parent_bid;
  fcb_t fcb[0];
} dir_t;
#define dir_check_magic(x) (((dir_t *) x)->magic == MAGIC_DIR)


extern sb_t *sb;
extern bid_t *fat;
extern dir_t *cur_dir;


int g3fs_loadfrom(const char *filename);
int g3fs_writeto(const char *filename);

int g3fs_mkdir(const char *path);
int g3fs_rmdir(const char *path);
int g3fs_ls();
int g3fs_cd(const char *path);
int g3fs_create(const char *path);
int g3fs_open(const char *path);
int g3fs_close(int);
int g3fs_write(int);
int g3fs_read(int);
int g3fs_rm(const char *path);
int g3fs_exit(const char *path);

bid_t find_free_block();
int find_available_fd();

#endif
