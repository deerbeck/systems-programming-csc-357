#include "utility.h"

// verbose flag
int v_flag = 0;
int b_processing = 0;

void print_open_fds(void)
{
    int fd;
    printf("Open file descriptors:\n");
    for (fd = 0; fd < MAX_FD; ++fd)
    {
        int flags = fcntl(fd, F_GETFD);

        if (flags != -1)
        {
            printf("%d\n", fd);
        }
    }
}

void sigintHandlerBatch(int sig_num)
{
    if (v_flag >= 2)
    {
        fprintf(stderr, "Caught signal 2 (Interrupt)\n");
    }
    exit(EXIT_SUCCESS);
}

void sigintHandlerCommand(int sig_num)
{
    if (v_flag >= 2)
    {
        fprintf(stderr, "Caught signal 2 (Interrupt)\n");
    }
    fprintf(stdout, "\n8-P ");
    // force that prompt in there
    fflush(stdout);
}

void sigintHandlerExecute(int sig_num)
{
    if (v_flag >= 2)
    {
        fprintf(stderr, "Caught signal 2 (Interrupt)\n");
    }
}

void sigintHandlerChild(int sig_num)
{
    // end childs life gracefully
    _exit(EXIT_SUCCESS);
}

pipeline get_command(FILE *input)
{
    // print out prompt if stdout and stdin are ttys
    if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO) && !b_processing)
    {
        fprintf(stdout, "8-P ");
    }

    // get command line
    char *command = readLongString(input);
    if (!command)
    {
        // EOF --> exit the shell
        exit(EXIT_SUCCESS);
    }

    // crack the string into the pipeline
    pipeline p_line = crack_pipeline(command);

    // check if valid pipeline (if not tell the user and get back)
    if (!p_line)
    {
        return NULL;
    }

    // print out pipeline in verboose mode
    if (v_flag >= 1)
    {
        print_pipeline(stderr, p_line);
    }

    // now give it back
    return p_line;
}