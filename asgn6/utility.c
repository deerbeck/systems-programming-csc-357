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

void sigintHandlerInteractive(int sig_num)
{
    if (v_flag >= 2)
    {
        fprintf(stderr, "Caught signal 2 (Interrupt)\n");
    }
    fprintf(stdout, "\n");
    fflush(stdout);
}

void cleanup(sigset_t old_mask, pipeline p_line)
{
    // unblock SIGINT
    // Restore the previous signal mask
    if (sigprocmask(SIG_SETMASK, &old_mask, NULL) == -1)
    {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
    if (v_flag >= 2)
    {
        fprintf(stderr,
                "SIGINT unblocked (pid = %d). \n", getpid());
    }
    free(p_line->cline);
    free_pipeline(p_line);
}

pipeline get_command(FILE *input)
{
    // print out prompt if stdout and stdin are ttys
    if (isatty(STDIN_FILENO) && isatty(STDOUT_FILENO) && !b_processing)
    {
        fprintf(stdout, "8-P ");
        fflush(stdout);
    }
    // get command line
    char *command = readLongString(input);
    if (!command)
    {
        // if it got interrupted
        if (ferror(input) && errno == EINTR)
        {
            // clear error flag and reset errno
            clearerr(input);
            errno = 0;
            return NULL;
        }
        else if (feof(input))
        {
            // EOF --> exit the shell
            exit(EXIT_SUCCESS);
        }
        // no EOF and no interrupt -> error doing stuff
        else
        {
            perror("stdin");
            exit(EXIT_FAILURE);
        }
    }

    // crack the string into the pipeline
    pipeline p_line = crack_pipeline(command);

    // check if valid pipeline
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