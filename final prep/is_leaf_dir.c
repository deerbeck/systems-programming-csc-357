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
    DIR *current;
    struct dirent *dirb;
    char pwd[PATH_MAX];
    current = opendir(path);
    
    if (!current)
        return 0;
    getcwd(pwd, PATH_MAX);
    if (!pwd)
        return 0;
    if (chdir(path) == -1)
        return 0;
    while ((dirb = readdir(current)) != NULL)
    {
        if (!strcmp(dirb->d_name, ".") || !strcmp(dirb->d_name, ".."))
            continue;
        if (stat(dirb->d_name, &sb) == -1)
            return 0;
        if (S_ISDIR(sb.st_mode))
            return 0;
    }
    if(chdir(pwd) == -1) return 0;
    return 1;
}
int main(int argc, char *argv[])
{
    printf("%d\n", is_leaf_directory("../lecture notes"));
    return 0;
}
