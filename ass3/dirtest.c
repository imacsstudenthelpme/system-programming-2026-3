#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int ac, char *av[])
{
    mkdir("-dirtest", 0700);
}