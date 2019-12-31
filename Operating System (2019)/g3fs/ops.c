#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "g3fs.h"


int g3fs_mkdir(const char *path) {
  int retval = -1;

  if (!check_filename_length(path)) {
    report_error("Filename too long");
  }

  if (!strcmp(path, "..")) {
    report_error("Filename cannot be `..'");
  }

  for (int i = 0; i < sb->fcb_num_per_block; ++i) {
    if (fcb_isexist(&cur_dir->fcb[i])) {
      if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH)) {
        report_error("File already existed");
      }
    }
  }

  if (cur_dir->item_num >= sb->fcb_num_per_block) {
    report_error("Current FCB is full, no more item is allowed");
  }

  bid_t bid;
  if ((bid = find_free_block()) == 0) {
    report_error("No more free space in the disk");
  }

  fat[bid] = BLK_END;

  fcb_t *fcb = &(cur_dir->fcb)[cur_dir->item_num];
  strncpy(fcb->fname, path, FNAME_LENGTH);
  fcb->size = 0;
  fcb->attrs = ISEXIST_MASK | ISDIR_MASK;
  fcb->bid = bid;
  cur_dir->item_num++;
  pwrite(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->bid));

  dir_t *new_dir = (dir_t *) calloc(1, sizeof(blk_t));
  new_dir->magic = MAGIC_DIR;
  new_dir->item_num = 0;
  new_dir->bid = bid;
  new_dir->parent_bid = cur_dir->bid;
  pwrite(fd, new_dir, sizeof(blk_t), offset_of(bid));
  free(new_dir);

  retval = 0;

 out:
  return retval;
}

int g3fs_rmdir(const char *path) {
  int retval = -1;

  if (!check_filename_length(path)) {
    report_error("Filename too long");
  }

  bool found = false;
  for (int i = 0; i < sb->fcb_num_per_block; ++i) {
    if (fcb_isexist(&cur_dir->fcb[i])) {
      if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH)) {
        found = true;

        if (!fcb_isdir(&cur_dir->fcb[i])) {
          g3fs_rm(cur_dir->fcb[i].fname);
        } else {
          g3fs_cd(cur_dir->fcb[i].fname);
          for (int j = 0; i < sb->fcb_num_per_block; ++j) {
            if (fcb_isexist(&cur_dir->fcb[j])) {
              if (!fcb_isdir(&cur_dir->fcb[j])) {
                g3fs_rm(cur_dir->fcb[j].fname);
              } else {
                g3fs_rmdir(cur_dir->fcb[j].fname);
              }
            } else {
              break;
            }
          }
          g3fs_cd("..");
        }

        bid_t bid, next_bid = cur_dir->fcb[i].bid;
        while (next_bid > 1) {
          bid = next_bid;
          next_bid = fat[bid];
          fat[bid] = BLK_FREE;
        }
        memset(&cur_dir->fcb[i], 0, sizeof(fcb_t));

        for (int j = i+1; j < sb->fcb_num_per_block; ++j) {
          if (fcb_isexist(&cur_dir->fcb[j])) {
            memcpy(&cur_dir->fcb[j-1], &cur_dir->fcb[j], sizeof(fcb_t));
            memset(&cur_dir->fcb[j], 0, sizeof(fcb_t));
          }
        }

        cur_dir->item_num--;
      }

    } else {
      break;
    }
  }

  if (!found) {
    report_error("Path not found");
  } else {
    pwrite(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->bid));
  }

  retval = 0;
 out:
  return retval;
}


int g3fs_ls() {
  for (int i = 0; i < cur_dir->item_num; ++i) {
    if (fcb_isexist(&cur_dir->fcb[i])) {
      printf("%9s [%s\tbid=%d\tsize=%d]\n",
             cur_dir->fcb[i].fname,
             (fcb_isdir(&cur_dir->fcb[i]) ? "dir" : "file" ),
             cur_dir->fcb[i].bid,
             cur_dir->fcb[i].size);
    } else {
      break;
    }
  }

  return 0;
}


int g3fs_cd(const char *path) {
  int retval = -1;

  if (!check_filename_length(path)) {
    report_error("Filename too long");
  }

  bool found = false;

  if (!strncmp(path, "..", FNAME_LENGTH)) {
    found = true;
    pread(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->parent_bid));
  }

  for (int i = 0; i < cur_dir->item_num; ++i) {
    if (fcb_isexist(&cur_dir->fcb[i])) {
      if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH)) {
        found = true;
        if (!fcb_isdir(&cur_dir->fcb[i])) {
          report_error("Cannot cd to a file");
        }
        pread(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->fcb[i].bid));
        break;
      }
    } else {
      break;
    }
  }

  if (!found) {
    report_error("Path not found");
  }

 out:
  return retval;
}


int g3fs_create(const char *path) {
  int retval = -1;

  if (!check_filename_length(path)) {
    report_error("Filename too long");
  }

  if (!strcmp(path, "..")) {
    report_error("Filename cannot be `..`");
  }

  if (cur_dir->item_num >= sb->fcb_num_per_block) {
    report_error("Current FCB is full, no more item is allowed");
  }

  bool found = false;
  for (int i = 0; i < sb->fcb_num_per_block; ++i) {
    if (fcb_isexist(&cur_dir->fcb[i])) {
      if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH)) {
        found = true;
        break;
      }
    }
  }
  if (found) {
    report_error("File already existed");
  }

  fcb_t *fcb = &cur_dir->fcb[cur_dir->item_num];
  strncpy(fcb->fname, path, FNAME_LENGTH);
  fcb->size = 0;
  fcb->bid = 0;
  fcb->attrs = ISEXIST_MASK;

  cur_dir->item_num++;

  pwrite(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->bid));

out:
  return retval;
}

int g3fs_open(const char *path) {
  int retval = -1;

  if (!check_filename_length(path)) {
    report_error("Filename too long");
  }

  bool found = false;
  for (int i = 0; i < cur_dir->item_num; ++i) {
    if (fcb_isexist(&cur_dir->fcb[i])) {
      if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH)) {
        found = true;
        if (fcb_isdir(&cur_dir->fcb[i])) {
          report_error("You cannot open a directory");
        }

        for (int index = 0; index < MAX_FD; ++index) {
          if (ofs[index].not_empty) {
            if (!strncmp(ofs[index].fcb.fname, path, FNAME_LENGTH)) {
              printf("fd is %d\n", index);
              retval = 0;
              goto out;
            }
          }
        }

        int available_fd = find_available_fd();
        if (available_fd < 0) {
          report_error("Too many opened files");
        }
        memcpy(&ofs[available_fd].fcb, &cur_dir->fcb[i], sizeof(fcb_t));
        ofs[available_fd].not_empty = true;
        ofs[available_fd].at_bid = cur_dir->bid;
        ofs[available_fd].fcb_id = i;
        ofs[available_fd].off = 0;
        ofs[available_fd].is_fcb_modified = false;
        printf("fd is %d\n", available_fd);
        break;
      }
    } else {
      break;
    }
  }

  if (!found) {
    report_error("File not found");
  }

  retval = 0;

 out:
  return retval;
}


int g3fs_close(int target_fd) {
  int retval = -1;

  if (target_fd > MAX_FD) {
    report_error("Illegal fd");
  }

  if (!ofs[target_fd].not_empty) {
    report_error("Illegal fd");
  }

  if (ofs[target_fd].is_fcb_modified) {
    dir_t *dir = (dir_t *) malloc(sizeof(blk_t));
    pread(fd, dir, sizeof(blk_t), offset_of(ofs[target_fd].at_bid));
    memcpy(&dir->fcb[ofs[target_fd].fcb_id], &ofs[target_fd].fcb, sizeof(fcb_t));
    pwrite(fd, dir, sizeof(blk_t), offset_of(dir->bid));

    if (ofs[target_fd].at_bid == cur_dir->bid) {
      memcpy(&cur_dir->fcb[ofs[target_fd].fcb_id], &ofs[target_fd].fcb, sizeof(fcb_t));
    }

    free(dir);
  }

  memset(&ofs[target_fd], 0, sizeof(of_t));

 out:
  return retval;
}


int g3fs_write(int target_fd) {
  int retval = -1;

  if (target_fd > MAX_FD) {
    report_error("Illegal fd");
  }

  if (!ofs[target_fd].not_empty) {
    report_error("Illegal fd");
  }

  char *buf = (char *) malloc(sizeof(blk_t));
  if (!gets(buf)) {
    report_error("Write something..");
  }

  bid_t bid = ofs[target_fd].fcb.bid;
  if (bid <= 1) {
    bid_t new_bid = find_free_block();
    if (!new_bid) {
      report_error("No free space");
    }
    if (bid) {
      ofs[target_fd].fcb.bid = new_bid;
    }
    fat[new_bid] = BLK_END;
    ofs[target_fd].fcb.bid = bid = new_bid;
    ofs[target_fd].is_fcb_modified = true;
  }

  off_t off = ofs[target_fd].off;
  while (off >= BLOCK_SIZE) {
    if (off == BLOCK_SIZE) {
      bid_t new_bid = find_free_block();
      if (!new_bid) {
        report_error("No free space");
      }
      if (bid) {
        fat[bid] = new_bid;
      }
      fat[new_bid] = BLK_END;
      bid = new_bid;
      off = 0;
    }
    bid = fat[bid];
    off -= BLOCK_SIZE;
  }

//  ofs[target_fd].fcb.size = 0;

  while (1) {
    if (strlen(buf) <= (BLOCK_SIZE - off)) {
      int actually_wrote = pwrite(fd, buf, strlen(buf), offset_of(bid) + off);
      ofs[target_fd].off += actually_wrote;
      ofs[target_fd].fcb.size += actually_wrote;
      ofs[target_fd].is_fcb_modified = true;
      break;

    } else {
      int actually_wrote = pwrite(fd, buf, BLOCK_SIZE - off, offset_of(bid) + off);
      ofs[target_fd].off += actually_wrote;
      ofs[target_fd].fcb.size += actually_wrote;
      bid_t new_bid = find_free_block();
      if (!new_bid) {
        report_error("No free space");
      }
      if (bid) {
        fat[bid] = new_bid;
      }
      fat[new_bid] = BLK_END;
      bid = new_bid;
      buf += BLOCK_SIZE - off;
      off = 0;
    }
  }

 out:
  if (buf)
    free(buf);
  return retval;
}

int g3fs_read(int target_fd) {
  int retval = -1;

  if (target_fd > MAX_FD) {
    report_error("Illegal fd");
  }

  if (!ofs[target_fd].not_empty) {
    report_error("Illegal fd");
  }

  printf("Read up to ? bytes: ");
  int count;
  scanf("%d", &count);

  bid_t bid = ofs[target_fd].fcb.bid;
  off_t off = ofs[target_fd].off;
  while (off > BLOCK_SIZE) {
    bid = fat[bid];
    off -= BLOCK_SIZE;
  }

  while (ofs[target_fd].off < ofs[target_fd].fcb.size) {
    if (count <= (BLOCK_SIZE - off)) {
      blk_t *blk = (blk_t *) malloc(sizeof(blk_t));
      pread(fd, blk, count, offset_of(bid) + off);
      write(0, blk, min(count, ofs[target_fd].fcb.size - ofs[target_fd].off));
      free(blk);
      ofs[target_fd].off += min(count, ofs[target_fd].fcb.size - ofs[target_fd].off);
      count = 0;
      break;

    } else {
      blk_t *blk = (blk_t *) malloc(sizeof(blk_t));
      pread(fd, blk, BLOCK_SIZE - off, offset_of(bid) + off);
      write(0, blk, BLOCK_SIZE - off);
      free(blk);
      ofs[target_fd].off += BLOCK_SIZE - off;
      count -= BLOCK_SIZE - off;
      // TODO
      off = 0;
    }
  }

  puts("");

 out:
  return retval;
}

int g3fs_rm(const char *path) {
  int retval = -1;

  if (!check_filename_length(path)) {
    report_error("Filename too long");
  }

  bool found = false;
  for (int i = 0; i < sb->fcb_num_per_block; ++i) {
    if (fcb_isexist(&cur_dir->fcb[i])) {
      if (!strncmp(cur_dir->fcb[i].fname, path, FNAME_LENGTH)) {
        found = true;

        if (fcb_isdir(&cur_dir->fcb[i])) {
          report_error("You cannot rm a directory, you should use rmdir");
        }

        fcb_t *fcb = &cur_dir->fcb[i];
        bid_t bid, next_bid = fcb->bid;
        while (next_bid > 1) {
          bid = next_bid;
          next_bid = fat[bid];
          fat[bid] = BLK_FREE;
        }
        memset(fcb, 0, sizeof(fcb_t));

        for (int j = i+1; j < sb->fcb_num_per_block; ++j) {
          if (fcb_isexist(&cur_dir->fcb[j])) {
            memcpy(&cur_dir->fcb[j-1], &cur_dir->fcb[j], sizeof(fcb_t));
            memset(&cur_dir->fcb[j], 0, sizeof(fcb_t));
          }
        }

        cur_dir->item_num--;
      }

    } else {
      break;
    }
  }

  if (!found) {
    report_error("Not found");
  }

  pwrite(fd, cur_dir, sizeof(blk_t), offset_of(cur_dir->bid));

  retval = 0;
 out:
  return retval;
}
