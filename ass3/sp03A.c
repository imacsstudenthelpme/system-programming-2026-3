#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <fcntl.h>

void do_ls(char[]);
void dostat(DIR *dir_ptr, char *filename);
void show_file_info(char *, struct stat *);
void mode_to_letters(int, char[]);
char *uid_to_name(uid_t);
char *gid_to_name(gid_t);

// ls -l : main -> do_ls -> dostat -> show_file_info
int main(int ac, char *av[])
{
    if (ac == 1)
        do_ls(".");
    else
        while (--ac) {
            printf("%s:\n", *++av);
            do_ls(*av);
        }
}

void do_ls(char dirname[])
{
    DIR *dir_ptr;
    struct dirent *direntp;

    if ((dir_ptr = opendir(dirname)) == NULL)
        fprintf(stderr, "ls: cannot open %s\n", dirname);
    else
    {
        while ((direntp = readdir(dir_ptr)) != NULL)
            dostat(dir_ptr, direntp->d_name);
        closedir(dir_ptr);
    }
}

void dostat(DIR *dir_ptr, char *filename)
{
    struct stat info;
    
    if (fstatat(dirfd(dir_ptr), filename, &info, AT_SYMLINK_NOFOLLOW) == -1)   
        fprintf(stderr, "error reading: %s : %s\n", filename, strerror(errno));          
    else                                  
        show_file_info(filename, &info);
}

void show_file_info(char *filename, struct stat *info_p)
{
    char modestr[11];

    mode_to_letters(info_p->st_mode, modestr);

    printf("%s", modestr);
    printf("%4d ", (int) info_p->st_nlink);
    printf("%-8s ", uid_to_name(info_p->st_uid));
    printf("%-8s ", gid_to_name(info_p->st_gid));
    printf("%8ld ", (long) info_p->st_size);
    printf("%.12s ", 4 + ctime(&info_p->st_mtime));
    printf("%s\n", filename);
}

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

char *uid_to_name(uid_t uid)
{
    struct passwd *getpwuid(uid_t), *pw_ptr;
    static char numstr[10];

    if ((pw_ptr = getpwuid(uid)) == NULL) {
        sprintf(numstr, "%d", uid);
        return numstr;
    }
    else
        return pw_ptr->pw_name;
}

char *gid_to_name(gid_t gid)
{
    struct group *getgrgid(gid_t), *grp_ptr;
    static char numstr[10];

    if ((grp_ptr = getgrgid(gid)) == NULL) {
        sprintf(numstr, "%d", gid);
        return numstr;
    }
    else
        return grp_ptr->gr_name;
}
