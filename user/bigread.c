#include "kernel/types.h"
#include "user/user.h"
#include "fcntl.h"

#define MAX_LINES 100
#define MAX_LINE_LEN 128

int
main(void) {
    char inputbuf[16];
    int n = 0, i, r;
    char c;
    int fd;
    char linebuf[MAX_LINE_LEN];
    int linepos = 0;
    int lines_read = 0;

    printf("Enter number of lines to read (1-100): ");

    // Read user input from stdin (fd 0)
    r = read(0, inputbuf, sizeof(inputbuf)-1);
    if (r <= 0) {
        printf("Failed to read input\n");
        exit(1);
    }
    inputbuf[r] = '\0';

    // Convert input string to integer
    for (i = 0; i < r && inputbuf[i] >= '0' && inputbuf[i] <= '9'; i++) {
        n = n * 10 + (inputbuf[i] - '0');
    }

    if (n < 1 || n > MAX_LINES) {
        printf("Invalid number of lines\n");
        exit(1);
    }

    fd = open("Demand.txt", 0);  // open file for reading
    if (fd < 0) {
        printf("Failed to open file\n");
        exit(1);
    }

    printf("Reading %d lines from Demand.txt:\n", n);

    // Read file character by character and print lines
    while (lines_read < n) {
        r = read(fd, &c, 1);
        if (r <= 0) {
            // EOF or error before reading all lines
            break;
        }
        if (c == '\n' || linepos == MAX_LINE_LEN - 1) {
            linebuf[linepos] = '\0';
            printf("%s\n", linebuf);
            linepos = 0;
            lines_read++;
        } else {
            linebuf[linepos++] = c;
        }
    }

    // If last line doesn't end with newline but has chars, print it
    if (linepos > 0 && lines_read < n) {
        linebuf[linepos] = '\0';
        printf("%s\n", linebuf);
    }

    close(fd);
    exit(0);
}
