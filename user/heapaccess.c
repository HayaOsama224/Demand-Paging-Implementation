#include "kernel/types.h"
#include "user/user.h"

int main() {
    char *arr = sbrk(4096 * 100); // allocate 100 pages
    arr[0] = 'X';                 // page 0
    arr[4096 * 10] = 'Y';         // page 10
    arr[4096 * 99] = 'Z';         // page 99
    printf("%x %x %x\n", arr[0], arr[4096 * 10], arr[4096 * 99]);
    exit(0);
}
