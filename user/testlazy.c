#include "kernel/types.h"
#include "user/user.h"

int main() {
    char *p = sbrk(4096); // allocate one page
    if (p == (char *)-1) {
        printf("sbrk failed\n");
        exit(1);
    }
    printf("Allocated page at %p\n", p);

    p[0] = 'Z';
    printf("Wrote %c at %p\n", p[0], p);

    exit(0);
}

