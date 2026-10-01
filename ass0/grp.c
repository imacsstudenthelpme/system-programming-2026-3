#include <grp.h>
#include <stdio.h>

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
