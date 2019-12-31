#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>


char buf[0x1001] = {0};

long argc = 0;
char *argv[0x1000] = {0};

int parse_args(void) {
  printf("$ ");
  int retval = -EFAULT;

  retval = read(0, buf, 0x1000);
  if (retval == 0) {
    puts("Good bye!");
    exit(0);
  }
  if (retval < 0)
    goto out;

  char *pbuf = buf, *pbuf_s = buf;
  char **pargv = argv;
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
  argc = pargv - argv;
  *pargv++ = NULL;

out:
  if (retval < 0) {
    printf("parse argument failed: %s\n", strerror(-retval));
  }
  return retval;
}


int do_fork(void) {
  int retval = -1;

  if (argc < 1) {
    // puts("[do_fork] input something then retry!");
    retval = -1;
    goto out;
  }

  retval = 0;
  pid_t pid = fork();
  if (pid == 0) {
    execvp(argv[0], argv);
    // will only be executed if execvp failed
    printf("execvp failed: %s\n", strerror(-errno));
    exit(-errno);

  } else if (pid < 0) {
    printf("fork failed: %s\n", strerror(-errno));
    retval = pid;
    goto out;
  }

out:
  return retval;
}

int collect_child_info() {
  int retval;
  int subprocess_retval = 0;

  retval = wait(&subprocess_retval);
  subprocess_retval = WEXITSTATUS(subprocess_retval);
  if (retval == -1 && errno) {
    printf("failed when collecting child process info: %s\n", strerror(-errno));
    retval = -errno;
  }
  printf("child returned with %d\n", subprocess_retval);

out:
  return retval;
}

int main() {
  int retval = -EFAULT;

  setvbuf(stdout, 0, _IONBF, 0);
  setvbuf(stderr, 0, _IONBF, 0);

  while (1) {
    if ((retval = parse_args()) < 0)
      goto out;

    if (do_fork() < 0)
      continue;

    if (collect_child_info() < 0)
      continue;

  }

out:
  return retval;
}


