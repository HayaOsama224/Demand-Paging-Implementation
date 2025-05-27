#include "kernel/types.h"
#include "user/user.h"

int main() {
    char *p = sbrk(4096 * 5);

    int pid = fork();
    if (pid == 0) {
        p[0] = 'C';
        printf("Child wrote: %x\n", p[0]);
        exit(0);
    } else {
        wait(0);
        p[4096 * 4] = 'P';
        printf("Parent wrote: %x\n", p[4096 * 4]);
    }

    exit(0);
}
