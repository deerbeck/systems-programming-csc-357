#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    struct stat sb;
    stat(argv[0], &sb);
    uid_t test = getuid();
    if (setuid(10) == -1)
        printf("Hello");
    test = getuid();
    return 0;
}