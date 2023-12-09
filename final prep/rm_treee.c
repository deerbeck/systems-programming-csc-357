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

int countfiles(const char *victim)
{
    struct stat sb;
    int ressult = 0;

    if (stat(victim, &sb) == -1)
        return -1;
    if (S_ISDIR(sb.st_mode))
    {
        int buf = 0;
        DIR *current;
        struct dirent *dirb;
        char pwd[PATH_MAX];
        current = opendir(victim);
        if (!current)
            return -1;
        getcwd(pwd, PATH_MAX);
        if (!pwd)
            return -1;
        if (chdir(victim) == -1)
            return -1;
        while ((dirb = readdir(current)) != NULL)
        {
            if (!strcmp(dirb->d_name, ".") || !strcmp(dirb->d_name, ".."))
                continue;

            if ((buf = countfiles(dirb->d_name)) == -1)
                return -1;
            ressult += buf;
        }
        if (chdir(pwd) == -1)
            return -1;
        if (closedir(current) == -1)
            return -1;
    }
    else
    {
        ressult += 1;
    }
    return ressult;
}

int main(int argc, char *argv[])
{
    printf("%d\n", countfiles("Test"));
    return 0;
}
