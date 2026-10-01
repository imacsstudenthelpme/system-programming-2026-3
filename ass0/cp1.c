#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096
#define COPYMODE 0644
#define CREAT_FLAG O_CREAT | O_WRONLY | O_TRUNC

void oops(char *s1, char *s2)
{
    fprintf(stderr, "error: %s", s1);
    perror(s2);
    exit(1);
}

int main(int argc, char *argv[])
{
    int fd_in, fd_out, n_chars;
    char buf[BUFFER_SIZE];
    if (argc != 3)
    {
        fprintf(stderr,"usage:%s source destination\n",argv[0]);
        exit(1);
    }

    if ((fd_in = open(argv[1], O_RDONLY)) == -1)
        oops("Cannot open", argv[1]);

    if ((fd_out = open(argv[2], CREAT_FLAG, COPYMODE)) == -1)
        oops("Cannot creat", argv[2]);

    while ((n_chars = read(fd_in, buf, BUFFER_SIZE)) > 0)
    {
        if ((write(fd_out, buf, n_chars)) != n_chars)
            oops("write error to ", argv[2]);
        if (n_chars == -1)
            oops("read error from", argv[1]);
        if (close(fd_in) == -1 || close(fd_out) == -1)
            oops("error closing file","");
    }
}
