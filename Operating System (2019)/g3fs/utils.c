#include "g3fs.h"


bid_t find_free_block() {
  for (int i = sb->data_start_bid; i < sb->total_block_num; ++i) {
    if (fat[i] == BLK_FREE) {
      return i;
    }
  }
  return 0;
}

int find_available_fd() {
  for (int i = 0; i < MAX_FD; ++i) {
    if (!ofs[i].not_empty) {
      return i;
    }
  }
  return -1;
}
