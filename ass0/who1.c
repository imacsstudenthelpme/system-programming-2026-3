#include <stdio.h>
#include <utmp.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TMP_FILE WTMP_FILE
#define SHOWHOST

void show_info(struct utmp * utbufp)
{
	printf("%-8.8s", utbufp->ut_user);
	printf(" %-8.8s", utbufp->ut_line);
	printf(" %10ld", utbufp->ut_time);
#ifdef SHOWHOST
	printf(" (%s)", utbufp->ut_host);
#endif
	printf("\n");
}


int main()
{
	struct utmp current_record;
	int utmpfd;
	int reclen = sizeof(current_record);

	if ((utmpfd = open(TMP_FILE, O_RDONLY)) == -1)
	{
		perror(TMP_FILE);
		exit(1);
	}

	while ( read(utmpfd, &current_record, reclen) == reclen)
		show_info(&current_record);
	close(utmpfd);
	return 0;
}
