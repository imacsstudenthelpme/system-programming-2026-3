#include <pwd.h>
#include <stdio.h>

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
