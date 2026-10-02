#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>

void mode_to_letters(int mode, char str[])
{
    strcpy(str, "----------");

    if (S_ISDIR(mode)) str[0] = 'd';
    if (S_ISCHR(mode)) str[0] = 'c';  
    if (S_ISBLK(mode)) str[0] = 'b';  
    if (S_ISFIFO(mode)) str[0] = 'f';
    if (S_ISLNK(mode))  str[0] = 'l';
    if (S_ISSOCK(mode)) str[0] = 's';

    if (mode & S_IRUSR) str[1] = 'r';    
    if (mode & S_IWUSR) str[2] = 'w';
    if (mode & S_IXUSR) str[3] = 'x';

    if (mode & S_IRGRP) str[4] = 'r';     
    if (mode & S_IWGRP) str[5] = 'w';
    if (mode & S_IXGRP) str[6] = 'x';

    if (mode & S_IROTH) str[7] = 'r';     
    if (mode & S_IWOTH) str[8] = 'w';
    if (mode & S_IXOTH) str[9] = 'x';
}

int main(int ac, char *av[])
{
    DIR *dir_p;
    struct dirent *dir_ent;

    for (int i = 1; i < ac; i++)
    {
        if ((dir_p = opendir(av[i])) != NULL)
        {
            while ((dir_ent = readdir(dir_p)) != NULL)
            {
                struct stat statbuf;
                char str[100];
                printf("inode %lu : \n", dir_ent->d_ino);
                printf("name : %s\n", dir_ent->d_name);
                lstat(dir_ent->d_name, &statbuf);
                mode_to_letters(statbuf.st_mode, str);
                printf("%s\n", str);
            }
        }
        else
            perror("fucked up");
    }
}