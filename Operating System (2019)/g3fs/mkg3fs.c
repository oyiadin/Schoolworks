#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "g3fs.h"

int main(int argc, char const *argv[]) {
  int retval = -1;

  if (argc < 3) {
    puts("Usage: ./mkg3fs filename total_size");
    goto out;
  }

  int total_size = atoi(argv[2]);
  if (total_size < 0 || total_size > 268435456) {
    puts("total_size too small or too large");
    retval = -1;
    goto out;
  }
  if (total_size % BLOCK_SIZE) {
    puts("total_size must be aligned with 4KB");
    retval = -1;
    goto out;
  }

  blk_t *blk = (blk_t *) malloc(sizeof(blk_t));
  int fd = open(argv[1], O_WRONLY | O_CREAT);

  // super block
  memset(blk, 0, sizeof(blk_t));
  sb_t *sb = (sb_t *) blk;
  sb->magic = MAGIC_SUPERBLOCK;
  sb->total_size = total_size;
  sb->total_block_num = total_size / BLOCK_SIZE;
  sb->fat_block_num = ((sb->total_block_num - 1 ) / (BLOCK_SIZE / 2)) + 1;
  sb->fcb_num_per_block = (BLOCK_SIZE - sizeof(dir_t)) / sizeof(fcb_t);
  sb->data_start_bid = 2 * sb->fat_block_num + 2;
  sb->fat_crc = 0;
  write(fd, sb, sizeof(blk_t));
  puts("superblock ok");

  sb = malloc(sizeof(sb_t));
  memcpy(sb, blk, sizeof(sb_t));

  // fat1
  memset(blk, 0, sizeof(blk_t));
  bid_t *fat = (bid_t *) blk;
  for (int i = 0; i < sb->data_start_bid; ++i) {
    fat[i] = BLK_END;
  }
  write(fd, fat, sizeof(blk_t));
  memset(blk, 0, sizeof(blk_t));
  for (int i = 1; i < sb->fat_block_num; ++i) {
    write(fd, fat, sizeof(blk_t));
  }
  puts("fat1 ok");

  // fat2
  for (int i = 0; i < sb->data_start_bid; ++i) {
    fat[i] = BLK_END;
  }
  write(fd, fat, sizeof(blk_t));
  memset(blk, 0, sizeof(blk_t));
  for (int i = 1; i < sb->fat_block_num; ++i) {
    write(fd, fat, sizeof(blk_t));
  }
  puts("fat2 ok");

  // root directory
  memset(blk, 0, sizeof(blk_t));
  dir_t *root_dir = (dir_t *) blk;
  root_dir->magic = MAGIC_DIR;
  root_dir->item_num = 0;
  root_dir->bid = sb->data_start_bid-1;
  root_dir->parent_bid = sb->data_start_bid-1;
  write(fd, root_dir, sizeof(blk_t));
  puts("root directory ok");

  // remains
  memset(blk, 0, sizeof(blk_t));
  for (int i = 2 + sb->fat_block_num; i < sb->total_block_num; ++i) {
    write(fd, blk, sizeof(blk_t));
  }
  puts("remains ok");

  close(fd);

  return 0;

 out:
  free(sb);
  free(blk);

  return retval;
}
