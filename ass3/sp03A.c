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
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

int do_dir_ls(char[], int *);
int get_filenames(DIR *, char ***, int *);
int dostat(DIR *, char *);
void show_file_info(char *, struct stat *, int);
void mode_to_letters(int, char[]);
char *uid_to_name(uid_t);
char *gid_to_name(gid_t);
char *join_path(char[], char[]);

// ls -alR : main -> do_dir_ls -> dostat -> show_file_info
int main(int ac, char *av[])
{
    struct stat finfo;
    int printed_dir = 0;
    int status = 0;

    if (ac == 1)
        return do_dir_ls(".", &printed_dir);

    for (int i = 1; i < ac; i++)
    {
        if (lstat(av[i], &finfo) == -1)
        {
            fprintf(stderr, "%s: %s\n", av[0], strerror(errno));
            status = 1;
        }
        else if (S_ISDIR(finfo.st_mode))
        {
            if (do_dir_ls(av[i], &printed_dir) != 0)
                status = 1;
        }
        else
            show_file_info(av[i], &finfo, AT_FDCWD);
    }

    return status;
}

int cmp(const void *s1, const void *s2)
{
    return strcmp(*(const char **)s1, *(const char **)s2);
}

int get_filenames(DIR *dir_ptr, char ***filenames, int *fileidx)
{
    struct dirent *direntp;
    char **temp;
    int capacity = 0;

    errno = 0;
    while ((direntp = readdir(dir_ptr)) != NULL)
    {
        if (*fileidx == capacity)
        {
            capacity += 10;
            temp = realloc(*filenames, capacity * sizeof(char *));
            if (temp == NULL)
            {
                perror("realloc");
                return 1;
            }
            *filenames = temp;
        }

        (*filenames)[*fileidx] = strdup(direntp->d_name);
        if ((*filenames)[*fileidx] == NULL)
        {
            perror("strdup");
            return 1;
        }
        (*fileidx)++;
        errno = 0;
    }

    if (errno != 0)
    {
        fprintf(stderr, "error reading directory : %s\n", strerror(errno));
        return 1;
    }

    qsort(*filenames, (size_t)(*fileidx), sizeof(char *), cmp);
    return 0;
}

int do_dir_ls(char dirname[], int *printed_dir)
{
    DIR *dir_ptr;
    struct stat info;
    char **filenames = NULL;
    char *path;
    int fileidx = 0;
    int status = 0;

    if ((dir_ptr = opendir(dirname)) == NULL)
    {
        fprintf(stderr, "ls: cannot open %s : %s\n", dirname, strerror(errno));
        return 1;
    }

    if (*printed_dir)
        printf("\n");
    printf("%s:\n", dirname);
    *printed_dir = 1;

    if (get_filenames(dir_ptr, &filenames, &fileidx) == 0)
    {
        for (int i = 0; i < fileidx; i++)
        {
            if (dostat(dir_ptr, filenames[i]) != 0)
                status = 1;
        }

        for (int i = 0; i < fileidx; i++)
        {
            if (strcmp(filenames[i], ".") == 0 || strcmp(filenames[i], "..") == 0)
                continue;

            if (fstatat(dirfd(dir_ptr), filenames[i], &info, AT_SYMLINK_NOFOLLOW) == -1)
            {
                fprintf(stderr, "error reading: %s : %s\n", filenames[i], strerror(errno));
                status = 1;
                continue;
            }
            if (!S_ISDIR(info.st_mode))
                continue;

            path = join_path(dirname, filenames[i]);
            if (path == NULL)
            {
                perror("malloc");
                status = 1;
                continue;
            }
            if (do_dir_ls(path, printed_dir) != 0)
                status = 1;
            free(path);
        }
    }
    else
        status = 1;

    for (int i = 0; i < fileidx; i++)
        free(filenames[i]);
    free(filenames);
    if (closedir(dir_ptr) == -1)
    {
        fprintf(stderr, "%s: %s\n", dirname, strerror(errno));
        status = 1;
    }
    return status;
}

int dostat(DIR *dir_ptr, char *filename)
{
    struct stat info;

    if (fstatat(dirfd(dir_ptr), filename, &info, AT_SYMLINK_NOFOLLOW) == -1)
    {
        fprintf(stderr, "error reading: %s : %s\n", filename, strerror(errno));
        return 1;
    }

    show_file_info(filename, &info, dirfd(dir_ptr));
    return 0;
}

void show_file_info(char *filename, struct stat *info_p, int dir_fd)
{
    char modestr[11];
    char target[PATH_MAX + 1];
    ssize_t target_len;

    mode_to_letters(info_p->st_mode, modestr);

    printf("%s", modestr);
    printf("%4d ", (int) info_p->st_nlink);
    printf("%-8s ", uid_to_name(info_p->st_uid));
    printf("%-8s ", gid_to_name(info_p->st_gid));
    printf("%8lld ", (long long) info_p->st_size);
    printf("%.12s ", 4 + ctime(&info_p->st_mtime));
    printf("%s", filename);

    if (S_ISLNK(info_p->st_mode))
    {
        target_len = readlinkat(dir_fd, filename, target, sizeof(target) - 1);
        if (target_len != -1)
        {
            target[target_len] = '\0';
            printf(" -> %s", target);
        }
    }
    printf("\n");
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

char *join_path(char dirname[], char filename[])
{
    size_t length = strlen(dirname) + strlen(filename) + 2;
    char *path = malloc(length);

    if (path != NULL)
    {
        strcpy(path, dirname);
        strcat(path, "/");
        strcat(path, filename);
    }
    return path;
}
