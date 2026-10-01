#include <stdio.h>
#include <utmp.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "utmplib.c"

#define TMP_FILE WTMP_FILE
#ifndef NULLUT
# define NULLUT ((struct *umtp)NULL)
#endif

// #define SHOWHOST

void show_info(struct utmp * utbufp)
{
	if (utbufp->ut_type != USER_PROCESS)
		return ;
	printf(" %-8.8s", utbufp->ut_user);
	printf(" %-8.8s", utbufp->ut_line);
	printf(" %12.12s", ctime(&(utbufp->ut_time)) + 4);
#ifdef SHOWHOST
	printf(" (%s)", utbufp->ut_host);
#endif
	printf("\n");
}


int main()
{
	struct utmp *utbufp;

	if (utmp_open(TMP_FILE) == -1)
	{
		perror(TMP_FILE);
		exit(1);
	}

	while ((utbufp = utmp_next()) != NULLUT)
	{
		show_info(utbufp);
	}
	utmp_close();
	clock_t fin = clock();
}
