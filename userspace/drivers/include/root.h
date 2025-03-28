#include <unistd.h>
#include <stdio.h>

static inline int is_root() {
  if (getegid() != 0) {
    printf("Did you forget to sudo?\n");
    return -1;
  }
  return 0;
}