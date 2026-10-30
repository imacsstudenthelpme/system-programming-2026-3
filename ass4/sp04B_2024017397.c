#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(void)
{
    size_t size = 128;
    char *buf;

    while (1) {
        buf = malloc(size);
        if (buf == NULL) {
            perror("malloc");
            return 1;
        }

        if (getcwd(buf, size) != NULL) {
            printf("%s\n", buf);
            free(buf);
            return 0;
        }

        free(buf);
        if (errno != ERANGE) {
            perror("getcwd");
            return 1;
        }
        if (size > (size_t)-1 / 2) {
            fprintf(stderr, "Current directory path is too long\n");
            return 1;
        }
        size = size * 2;
    }
}
