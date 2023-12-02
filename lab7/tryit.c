#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    // i hope i can use the program right
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s command \n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // init variables
    int status;
    pid_t child;

    // give birth
    child = fork();

    // did I birth correctly?
    if (child == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (child == 0)
    {
        // child

        // do stuff my child
        execl(argv[1], argv[1], NULL);

        // we only land here if execl fails
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }
    else
    {
        // mom I found you *-*
        // parent

        // wait for our child to die.... or exit life?
        wait(&status);

        // tell the world how our child did
        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status) == 0)
            {
                // it did good
                printf("Process %d succeeded.\n", child);
            }
            else
            {
                // it did no good
                printf("Process %d exited with an error value.\n", child);
            }

            // go as our child did go
            exit(WEXITSTATUS(status));
        }
        else
        {
            // child went abnormally
            fprintf(stderr, "Child process ended abnormally.\n");
            exit(EXIT_FAILURE);
        }
    }
}