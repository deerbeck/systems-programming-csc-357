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

int is_leaf_directory(const char *path)
{
    struct stat sb;
    int res = 1;
    if (stat(path, &sb) == -1)
        res = 0;
    if (!S_ISDIR(sb.st_mode))
        res = 0;

    char cwd[PATH_MAX + 1];
    getcwd(cwd, PATH_MAX);
    if (!cwd)
        res = 0;
    cwd[PATH_MAX] = '\0';
    if (chdir(path) == -1)
        res = 0;
    DIR *current;
    struct dirent *dirb;

    current = opendir(".");
    if (!current)
        res = 0;

    while ((dirb = readdir(current)) != NULL)
    {
        if (!strcmp(dirb->d_name, ".") || !strcmp(dirb->d_name, ".."))
            continue;
        if (stat(dirb->d_name, &sb) == -1)
            res = 0;
        if (S_ISDIR(sb.st_mode))
            res = 0;
    }
    if (closedir(current) == -1)
        res = 0;
    if (chdir(cwd) == -1)
        res = 0;
    return res;
}

int main(int argc, char *argv[])
{
    printf("%d\n", is_leaf_directory("Test/asdf/kassdlf"));
    return 0;
}
