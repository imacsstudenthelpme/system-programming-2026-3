#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


void mode_to_strs(int mode, char type_rst[], char mode_rst[])
{
    if (S_ISREG(mode)) strcpy(type_rst, "regular file");
    if (S_ISDIR(mode)) strcpy(type_rst, "directory");
    if (S_ISCHR(mode)) strcpy(type_rst, "character device");  
    if (S_ISBLK(mode)) strcpy(type_rst, "block device");  
    if (S_ISFIFO(mode)) strcpy(type_rst, "FIFO (named pipe)");
    if (S_ISLNK(mode)) strcpy(type_rst, "symbolic link");
    if (S_ISSOCK(mode)) strcpy(type_rst, "socket");

    mode_rst[0] = ((mode & 00700) >> 6) + '0'; //user
    mode_rst[1] = ((mode & 00070) >> 3) + '0'; //group
    mode_rst[2] = ((mode & 00007)) + '0'; // others
    mode_rst[3] = 0;
}

void show_file_info(char *filename, struct stat *stat_p)
{
    char type_str[18];
    char mode_str[4];

    mode_to_strs(stat_p->st_mode, type_str, mode_str);
    printf("pathname: \"%s\"\n", filename);
    printf("type: %s\n", type_str);
    printf("filemode: %s\n", mode_str);
    printf("inode #: %ld\n", stat_p->st_ino);
    printf("number of linkes = %d\n", stat_p->st_nlink);
    printf("uid: %d\n", stat_p->st_uid);
    printf("gid = %d\n", stat_p->st_gid);
    printf("size = %ld\n", stat_p->st_size);
    printf("preferred I/O block size = %d\n", stat_p->st_blksize);
    printf("number of 512-byte blocks = %ld\n", stat_p->st_blocks);
    printf("-----------------------------------------\n");

}

int main(int ac, char *av[])
{
    struct stat statbuf;
    
    for (int i = 1; i < ac; i++)
    {
        if(lstat(av[i], &statbuf) == -1)
        {
            perror(av[i]);
            continue;
        }
        show_file_info(av[i], &statbuf);
    }
}