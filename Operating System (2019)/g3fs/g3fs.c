#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "g3fs.h"


char buf[0x101] = {0};
int parsed_argc = 0;
char *parsed_argv[0x100] = {0};

int parse_args(void) {
  printf("$ ");
  int retval = -1;

  retval = read(0, buf, 0x100);
  if (retval == 0) {
    puts("Good bye!");
    exit(0);
  }
  if (retval < 0)
    goto out;

  char *pbuf = buf, *pbuf_s = buf;
  char **pargv = parsed_argv;
  bool stopped = false;
  while (!stopped) {
    switch (*pbuf) {
      case '\0':
      case '\n':
      case '\r':
        stopped = true;
      case ' ':
        if (pbuf - pbuf_s > 0) {
          *pargv++ = pbuf_s;
          *pbuf = '\0';
        }
        pbuf_s = pbuf+1;
        break;
    }
    ++pbuf;
  }

  retval = 0;
  parsed_argc = pargv - parsed_argv;
  *pargv++ = NULL;

out:
  if (retval < 0)
    printf("parse argument failed: %s\n", strerror(-retval));
  return retval;
}

struct cmd_map {
  char *cmd;
  void (*func)();
  int type;
};

struct cmd_map cmd_maps[] = {
  {"mkdir", (void (*) ()) g3fs_mkdir, 1},
  {"rmdir", (void (*) ()) g3fs_rmdir, 1},
  {"ls", (void (*) ()) g3fs_ls, 0},
  {"cd", (void (*) ()) g3fs_cd, 1},
  {"touch", (void (*) ()) g3fs_create, 1},
  {"create", (void (*) ()) g3fs_create, 1},
  {"open", (void (*) ()) g3fs_open, 1},
  {"close", (void (*) ()) g3fs_close, 10},
  {"write", (void (*) ()) g3fs_write, 10},
  {"read", (void (*) ()) g3fs_read, 10},
  {"rm", (void (*) ()) g3fs_rm, 1},
  {"exit", (void (*) ()) g3fs_exit, -1},
  {NULL, NULL, 0}
};



int main(int argc, char const *argv[]) {
  int retval = -1;

  setvbuf(stdout, 0, _IONBF, 0);
  setvbuf(stderr, 0, _IONBF, 0);

  if (argc < 2) {
    puts("Usage: ./gsfs filename");
    goto out;
  }

  if (access(argv[1], F_OK) < 0) {
    puts("Image file not found");
    retval = -1;
    goto out;
  }

  g3fs_loadfrom(argv[1]);

  while (1) {
    retval = parse_args();
    if (retval < 0)
      goto out;

    int i = 0;
    char *cmd = parsed_argv[0];
    while (cmd && cmd_maps[i].cmd) {
      if (!strcmp(cmd_maps[i].cmd, cmd)) {
        switch (cmd_maps[i].type) {
          case 0:
            ((int (*)())(cmd_maps[i].func))();
            break;
          case 1:
            if (parsed_argc < 2) {
              puts("Too few arguments");
              break;
            }
            ((int (*)(char *, ...))(cmd_maps[i].func))(parsed_argv[1]);
            break;
          case 10:
            if (parsed_argc < 2) {
              puts("Too few arguments");
              break;
            }
            ((int (*)(int, ...))(cmd_maps[i].func))(atoi(parsed_argv[1]));
            break;
          case -1:
            ((int (*)(const char *, ...))(cmd_maps[i].func))(argv[1]);
            break;
        }
        break;
      }
      ++i;
    }
    if (i + 1 >= (sizeof(cmd_maps) / sizeof(struct cmd_map))) {
      puts("Unknown command");
    }
  }

 out:
  return retval;
}
