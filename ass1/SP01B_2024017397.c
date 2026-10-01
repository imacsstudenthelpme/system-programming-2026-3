#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUFFERSIZE 10000

typedef struct _error_info
{
    char *fileName;
    int errNum;
} error_info;

int main(int argc, char *argv[])
{
    int fd, errIdx = 0, hadError = 0;
    ssize_t readAmount, writeAmount;
    char *fileName;
    char buf[BUFFERSIZE];
    
    if (argc == 1)
    {
        fd = STDIN_FILENO;

        while ((readAmount = read(fd, buf, BUFFERSIZE)) > 0)
        {
            if ((writeAmount = write(STDOUT_FILENO, buf, readAmount)) == -1)
            {
                perror("Error writing file ");
                break;
            }
        }
        if (readAmount == -1)
        {
            perror("Error writing file ");
            return 1;
        }
        return 0;
    }
    
    error_info errList[argc - 1];

    for (int i = 1; i < argc; i++)
    {
        fileName = argv[i];
        if ((fd = open(fileName, O_RDONLY)) == -1)
        {
            errList[errIdx].errNum = errno;
            errList[errIdx++].fileName = fileName;
            hadError = 1;
            continue;
        }

        while ((readAmount = read(fd, buf, BUFFERSIZE)) > 0)
        {
            if ((writeAmount = write(STDOUT_FILENO, buf, readAmount)) == -1)
            {
                perror("Error writing file ");
                hadError = 1;
                break;
            }
        }
        if (readAmount == -1)
        {
            perror("Error reading file ");
            hadError = 1;
        }

        close(fd);
    } 

    for (int i = 0; i < errIdx; i++)
    {
        fprintf(stderr, "%s: %s: %s\n", argv[0], errList[i].fileName, 
            strerror(errList[i].errNum));
    }
    return hadError;
}