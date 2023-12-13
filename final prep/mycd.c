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

int main(int argc, char *argv[])
{
    chdir(argv[1]);
    char buffer[1000];
    getcwd(buffer, 1000);
    printf("%s",buffer);
    return 0;
}
