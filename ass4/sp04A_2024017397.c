#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <ftw.h>

static mode_t mode;
static int failed = 0;

static int change_mode(const char *path, const struct stat *st, int type, struct FTW *ftw)
{
    (void)st;
    (void)ftw;

    if (type == FTW_SL) return 0;
    if (type == FTW_NS || type == FTW_DNR) {
        fprintf(stderr, "%s: cannot access: %s\n", path, strerror(errno));
        failed = 1;
        return 0;
    }

    if (chmod(path, mode) == -1) {
        fprintf(stderr, "%s: chmod failed: %s\n", path, strerror(errno));
        failed = 1;
    } else
        printf("%s\n", path);

    return 0;
}

int main(int argc, char *argv[])
{
    char *end;
    unsigned long n;
    int i;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s mode file_or_directory...\n", argv[0]);
        return 1;
    }

    errno = 0;
    n = strtoul(argv[1], &end, 8);
    if (errno != 0 || argv[1][0] == '\0' || *end != '\0' || n > 07777) {
        fprintf(stderr, "%s: invalid octal mode\n", argv[1]);
        return 1;
    }
    mode = (mode_t)n;

    for (i = 2; i < argc; i++) {
        struct stat st;
        if (stat(argv[i], &st) == -1) {
            fprintf(stderr, "%s: %s\n", argv[i], strerror(errno));
            failed = 1;
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            // do the directory last so chmod doesn't block its contents
            if (nftw(argv[i], change_mode, 32, FTW_PHYS | FTW_DEPTH) == -1) {
                fprintf(stderr, "%s: traversal failed: %s\n", argv[i], strerror(errno));
                failed = 1;
            }
        } else {
            if (chmod(argv[i], mode) == -1) {
                fprintf(stderr, "%s: chmod failed: %s\n", argv[i], strerror(errno));
                failed = 1;
            } else
                printf("%s\n", argv[i]);
        }
    }
    return failed ? 1 : 0;
}
