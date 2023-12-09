#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

int make_new_log(char *fname);

int main(int argc, char *argv[])
{
    char *fname = "test";
    int fd;
    fd = make_new_log(fname);
    if (fd == -1)
    {
        printf("File already exists\n");
        exit(EXIT_SUCCESS);
    }
    close(fd);
    return 0;
}
int make_new_log(char *fname)
{
    int res = -1;
    if (access(fname, F_OK) == -1)
    {
        res = open(fname, O_WRONLY | O_CREAT,
                   (S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH));
    }
    return res;
}