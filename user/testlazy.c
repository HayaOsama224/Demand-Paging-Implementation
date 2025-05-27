#include "kernel/types.h"
#include "user/user.h"

#define PAGE_SIZE 4096
#define TOTAL_PAGES 5
#define ARRAY_SIZE (PAGE_SIZE * TOTAL_PAGES / sizeof(int))

int arr[ARRAY_SIZE];

void test_one_page() {
    int *p = (int *)sbrk(PAGE_SIZE);
    if (p == (int *)-1) {
        printf("sbrk failed\n");
        exit(1);
    }
    printf("Allocated page at %p\n", p);
    p[0] = 1;
    printf("Wrote %d at %p\n", p[0], p);
}

void test_two_pages() {
    int *p1 = (int *)sbrk(PAGE_SIZE);
    if (p1 == (int *)-1) {
        printf("sbrk for page 1 failed\n");
        exit(1);
    }
    printf("Allocated first page at %p\n", p1);
    p1[0] = 100;
    p1[(PAGE_SIZE / sizeof(int)) - 1] = 200;
    printf("Wrote %d and %d in first page\n", p1[0], p1[(PAGE_SIZE / sizeof(int)) - 1]);

    int *p2 = (int *)sbrk(PAGE_SIZE);
    if (p2 == (int *)-1) {
        printf("sbrk for page 2 failed\n");
        exit(1);
    }
    printf("Allocated second page at %p\n", p2);
    p2[0] = 300;
    p2[(PAGE_SIZE / sizeof(int)) - 1] = 400;
    printf("Wrote %d and %d in second page\n", p2[0], p2[(PAGE_SIZE / sizeof(int)) - 1]);
}

void lazy_test() {
    printf("Array declared with %lu ints (~%lu bytes)\n", (unsigned long)ARRAY_SIZE, (unsigned long)(ARRAY_SIZE * sizeof(int)));
    for (int i = 0; i < TOTAL_PAGES; i++) {
        int index = i * (PAGE_SIZE / sizeof(int));
        printf("Accessing index %d (page %d)...\n", index, i);
        arr[index] = i * 100;
        printf("Wrote %d at arr[%d] (%p)\n", arr[index], index, &arr[index]);
        sleep(30);
    }
    printf("Reading back values:\n");
    for (int i = 0; i < TOTAL_PAGES; i++) {
        int index = i * (PAGE_SIZE / sizeof(int));
        printf("arr[%d] = %d at %p\n", index, arr[index], &arr[index]);
    }
}

int main() {
    char buf[2];
    printf("Choose test:\n");
    printf("1 - Test single page allocation\n");
    printf("2 - Test two page allocations\n");
    printf("3 - Lazy allocation array test\n");
    printf("Enter choice: ");
    if (read(0, buf, 2) <= 0) {
        printf("Failed to read input.\n");
        exit(1);
    }
    if (buf[0] == '1') {
        test_one_page();
    } else if (buf[0] == '2') {
        test_two_pages();
    } else if (buf[0] == '3') {
        lazy_test();
    } else {
        printf("Invalid choice.\n");
    }
    exit(0);
}
