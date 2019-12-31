#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "g3fs.h"


int fd = -1;

sb_t *sb = NULL;
bid_t *fat = NULL;
dir_t *cur_dir = NULL;

of_t ofs[MAX_FD] = {0};


int g3fs_loadfrom(const char *filename) {
  int retval = -1;

  fd = open(filename, O_RDWR);
  struct stat stat_buf;
  if (stat(filename, &stat_buf) < 0) {
    printf("error when stat(): %s\n", strerror(-errno));
    retval = -errno;
    goto out;
  }
  if ((int) stat_buf.st_size < sizeof(blk_t)) {
    puts("file too small");
    retval = -1;
    goto out;
  }

  blk_t buf;
  retval = read(fd, &buf, sizeof(blk_t));

  sb_t *disk_sb = (sb_t *) &buf;
  if (!sb_check_magic(disk_sb)) {
    report_error("Magic number of superblock didn't match");
  }
  sb = malloc(sizeof(sb_t));
  memcpy(sb, disk_sb, sizeof(sb_t));

  if ((int) stat_buf.st_size < (sb->total_size - sizeof(blk_t))) {
    report_error("file too small");
  }

  fat = malloc(sizeof(blk_t) * sb->fat_block_num);
  read(fd, fat, sizeof(blk_t) * sb->fat_block_num);
  read(fd, fat, sizeof(blk_t) * sb->fat_block_num);

  cur_dir = malloc(sizeof(blk_t));
  read(fd, cur_dir, sizeof(blk_t));

 out:
  return retval;
}


int g3fs_writeto(const char *filename) {
  blk_t *blk = calloc(1, sizeof(blk_t));
  // super block
  memcpy(blk, sb, sizeof(sb_t));
  write(fd, blk, sizeof(blk_t));

  // fat1
  memcpy(blk, fat, sizeof(blk_t));
  write(fd, blk, sizeof(blk_t));
  // fat2
  write(fd, blk, sizeof(blk_t));

  free(blk);
  free(sb);
  free(fat);
  free(cur_dir);
  return 0;
}


int g3fs_exit(const char *filename) {
  g3fs_writeto(filename);
  exit(0);
}
