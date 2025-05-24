#include "kernel/types.h"
#include "user/user.h"

#define PAGESIZE 4096
#define NUM_PAGES 20

char *arr;

int
main()
{
  printf("testlazy3: starting test\n");

  // Allocate a large array (but don't touch it yet)
  arr = sbrk(NUM_PAGES * PAGESIZE);

  if (arr == (char *)-1) {
    printf("testlazy3: sbrk failed\n");
    exit(1);
  }

  // Touch every fifth page to trigger lazy loading
  for (int i = 0; i < NUM_PAGES; i += 5) {
    printf("Touching page %d at address %p\n", i, arr + i * PAGESIZE);
    arr[i * PAGESIZE] = 'X';
  }

  printf("testlazy3: done\n");
  exit(0);
}

