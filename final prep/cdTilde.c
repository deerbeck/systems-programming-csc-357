#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>

int cdTilde(const char *username)
{
    struct passwd *pw;
    pw = getpwnam((username+1));
    if (!pw)
        return -1;
    return chdir(pw->pw_dir);
}

int main(int argc, char *argv[])
{
    char buf[1000];
    printf("%s\n", getcwd(buf, 1000));
    cdTilde("~johannes");
    printf("%s\n", getcwd(buf, 1000));
}
