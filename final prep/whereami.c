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
char *whereami();

int main(int argc, char *argv[])
{
    char *buf;
    buf = whereami();
    if(! buf) return -1;
    printf("%s\n" , buf);
    free(buf);
    return 0;
}
char *whereami()
{
    char *path = NULL;
    int len = SIZE;
    while (1)
    {
        
        path = (char *)realloc(path, len);
        path = getcwd(path, len);
        if(path) break;
        else if (errno != ERANGE && errno != 0)
        {
            return NULL;
        }
        len += SIZE;
    }
        len = strlen(path) + 1;
        path = (char *)realloc(path, len);
    return path;
}