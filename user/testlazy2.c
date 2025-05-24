#include "kernel/types.h"
#include "user/user.h"

#define PGSIZE 4096
#define NUM_PAGES 5

int main() {
    printf("Demand paging test started\n");

    // Allocate NUM_PAGES pages
    char *base = sbrk(NUM_PAGES * PGSIZE);
    if (base == (char *)-1) {
        printf("sbrk failed\n");
        exit(1);
    }
    printf("Allocated %d pages starting at %p\n", NUM_PAGES, base);

    // Write a unique char to each page
    for (int i = 0; i < NUM_PAGES; i++) {
        char *addr = base + i * PGSIZE;
        char value = 'a' + i;
        *addr = value;
        printf("Writing: value=%d  at addr=%p\n", (int)value, addr);
    }

    // Read back and verify
    for (int i = 0; i < NUM_PAGES; i++) {
        char *addr = base + i * PGSIZE;
        char value = *addr;
        printf("Reading: value=%d  from addr=%p\n", (int)value, addr);
        if (value != 'a' + i) {
            printf("Value mismatch at page %d!\n", i);
            exit(1);
        }
    }

    printf("Demand paging test completed successfully\n");
    exit(0);
}
