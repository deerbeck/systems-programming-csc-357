#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define SIZE 10
int isroot(const char *path)
{
   struct stat sroot, sb;
   if(stat("/", &sroot) == -1) return 0;
   if(stat(path, &sb) == -1) return 0;
   return ((sroot.st_ino == sb.st_ino) && (sroot.st_dev == sb.st_dev));
}

int main(int argc, char *argv[])
{
    chdir("/");
    printf("%d\n", isroot("."));
    return 0;
}
