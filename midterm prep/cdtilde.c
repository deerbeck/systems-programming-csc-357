#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int cdTilde(const char *user)
{
    char buf[(strlen("/home/") + strlen(user) + 1)];
    strcpy(buf, "/home/");
    strcat(buf, user);

    if (chdir(buf) == -1)
        return -1;
    return 0;
}

int main(int argc, char *argv[])
{
    const char *username = "johanne";
    char buffer[300];
    if (cdTilde(username) == 0)
    {
        printf("Changed working directory to %s's home directory.\n", username);
        getcwd(buffer, 300);
        printf("%s\n", buffer);
    }
    else
    {
        fprintf(stderr, "Failed to change the working directory.\n");
    }
    printf("%d\n", 0100);
    return 0;
}