#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#define PATH_MAX 2048

int rm_tree(const char *victim)
{
    struct stat sb;
    if (stat(victim, &sb) == -1)
        return 0;
    if (!S_ISDIR(sb.st_mode))
    {
        if (unlink(victim) == -1)
            return 0;
        return 1;
    }
    else
    {
        char cwd[PATH_MAX + 1];
        getcwd(cwd, PATH_MAX);
        if (!cwd)
            return 0;
        cwd[PATH_MAX] = '\0';

        if (chdir(victim) == -1)
            return 0;
        DIR *current;
        current = opendir(".");
        if (!current)
            return 0;
        int res = 1;
        struct dirent *dirp;
        while ((dirp = readdir(current)) != NULL)
        {
            if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
                continue;

            if (stat(dirp->d_name, &sb) == -1)
                res = 0;
            if (S_ISDIR(sb.st_mode))
            {
                if (!rm_tree(dirp->d_name))
                    res = 0;
            }
            else
            {
                if (unlink(dirp->d_name) == -1)
                    res = 0;
            }
        }

        if (closedir(current) == -1)
            res = 0;
        if (chdir(cwd) == -1)
            res = 0;
        if (rmdir(victim) == -1)
            res = 0;
        return res;
    }
}

int main(int argc, char *argv[])
{
    printf("%d\n", rm_tree("Test"));
    return 0;
}
