#include "kernel/types.h"
#include "user/user.h"

int main() {
  char *p = (char *)0x400000; // Any high address in the lazy-mapped segment
  *p = 'Z';
  printf("Lazy page loaded: %c\n", *p);
  exit(0);
}

