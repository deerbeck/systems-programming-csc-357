#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    // greet the world
    printf("Hello, World!, \n");

    pid_t child = fork();
    if (child == -1)
    {
        // error
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (child == 0)
    {
        // child
        printf("This is the child, pid %d.\n", getpid());
        // child is done with this exhausting task
        exit(EXIT_SUCCESS);
    }
    else
    {
        // parent
        printf("This is the parent, pid %d.\n", getpid());

        // wait for the child
        // we don't really care about the status
        wait(NULL);

        // saying goodbye to this world
        printf("This is the parent, pid %d, signing off.\n", getpid());
    }

    // make compiler happy
    return 0;
}