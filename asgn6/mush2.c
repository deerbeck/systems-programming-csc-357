#include "utility.h"

// used for getopt (debug)
extern int optind, optopt, opterr;
extern int h_errno;

int main(int argc, char *argv[])
{
    // check if mush 2 is used correctly
    // parse command line arguments
    int opt;
    while ((opt = getopt(argc, argv, "v")) != -1)
    {
        switch (opt)
        {
        case 'v':
            v_flag += 1;
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [ -v ] [ infile ]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    char *infile;
    // default input is sdtin
    FILE *input = stdin;
    // batch processing
    if (optind == (argc - 1))
    {
        b_processing = 1;
        infile = argv[optind];
        if (((input = fopen(infile, "r")) == NULL))
        {
            perror("input");
            exit(EXIT_FAILURE);
        }
        // intruduce b_processing signal_handler
        signal(SIGINT, sigintHandlerBatch);
    }

    // init variables
    int i;
    int j;
    int status;
    int stages;
    // introduce signal handler to parent
    sigset_t new_mask, old_mask;

    // let the shell run as long as exit or SIGINT
    while (1)
    {
        // install command handler#
        if (!b_processing)
        {
            signal(SIGINT, sigintHandlerCommand);
        }
        // if so parse a command line
        pipeline p_line = get_command(input);

        // no comamnd so lets go back
        if (!p_line)
        {
            continue;
        }

        // install execute handler
        if (!b_processing)
        {
            signal(SIGINT, sigintHandlerExecute);
        }

        // Block SIGINT until children are sent out to the world
        sigemptyset(&new_mask);
        sigaddset(&new_mask, SIGINT);
        // Block SIGINT using sigprocmask
        if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) == -1)
        {
            perror("sigprocmask");
            exit(EXIT_FAILURE);
        }
        if (v_flag >= 2)
        {
            fprintf(stderr,
                    "SIGINT blocked. (pid = %d) \n", getpid());
        }

        // built in cd
        if (!strcmp(p_line->stage->argv[0], "cd"))
        {
            char *dest;

            // verbose
            if (v_flag >= 2)
            {
                fprintf(stderr, "Found builtin cd.\n");
            }
            // check if path provided
            if (p_line->stage->argc > 1)
            {
                // use provided path
                dest = p_line->stage->argv[1];
            }
            else
            {
                // if no path provided use env value
                dest = getenv("HOME");
            }
            if (!dest)
            {
                // inform the user
                fprintf(stderr, "No value for HOME in PATH.\n");
            }

            // verbose
            if (v_flag >= 3)
            {
                fprintf(stderr, "Attempting to cd to \"%s\"\n", dest);
            }

            // finally chdir
            if (chdir(dest) == -1)
            {
                perror(dest);
            }

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
            // free those pointers
            free(p_line->cline);
            free_pipeline(p_line);
            continue;
        }

        // use exit to get out
        if (!strcmp(p_line->stage->argv[0], "exit"))
        {
            if (v_flag >= 2)
            {
                fprintf(stderr, "Found builtin exit.\n");
            }
            // We are done
            yylex_destroy();
            // free those pointers
            free(p_line->cline);
            free_pipeline(p_line);
            exit(EXIT_SUCCESS);
        }

        // looks cleaner
        stages = p_line->length;

        // establish pipes (length of pipeline - 1)
        int pipes[stages - 1][2];
        for (i = 0; i < stages - 1; i++)
        {
            // check for success
            if (pipe(pipes[i]) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }
        if (v_flag >= 2)
        {
            for (j = 0; j < stages - 1; j++)
            {
                fprintf(stderr,
                        "Pipe filedescriptor: %d and %d\n",
                        pipes[j][READ_END],
                        pipes[j][WRITE_END]);
            }
        }
        // get our children (as many as length says us to get)
        pid_t children[stages];
        for (i = 0; i < stages; i++)
        {

            if ((children[i] = fork()) == 0)
            {
                int input = STDIN_FILENO;
                int output = STDOUT_FILENO;
                // Bock Sigint
                // block SIGINT during launch and setup
                if (v_flag >= 2)
                {
                    fprintf(stderr,
                            "SIGINT blocked. (pid = %d) \n", getpid());
                }
                // install default signalHandler
                signal(SIGINT, sigintHandlerChild);

                /* we know child copies memory --> index of for loop is stage
                 * of pipeline */
                // child, lets do stuff
                // first handle pipes (more than 1 stage)
                if (stages > 1)
                {
                    // if this is the frist child, dup stdout
                    if (i == 0)
                    {
                        if (dup2((pipes[i][WRITE_END]), STDOUT_FILENO) == -1)
                        {
                            perror("dup2");
                            _exit(EXIT_FAILURE);
                        }
                    }
                    // last child
                    else if (i == stages - 1)
                    {
                        /* last pipe is in last child index -1 (always one less
                         * pipe than children)*/
                        if (dup2((pipes[i - 1][READ_END]), STDIN_FILENO) == -1)
                        {
                            perror("dup2");
                            _exit(EXIT_FAILURE);
                        }
                    }
                    // for all the other children:
                    else
                    {
                        if (dup2((pipes[i - 1][READ_END]), STDIN_FILENO) == -1)
                        {
                            perror("dup2");
                            _exit(EXIT_FAILURE);
                        }
                        if (dup2((pipes[i][WRITE_END]), STDOUT_FILENO) == -1)
                        {
                            perror("dup2");
                            _exit(EXIT_FAILURE);
                        }
                    }

                    // organization is done, so clean up now
                    for (j = 0; j < stages - 1; j++)
                    {
                        // Close all pipe remaining pipe fds
                        if (close(pipes[j][READ_END]) == -1)
                        {
                            perror("close");
                            _exit(EXIT_FAILURE);
                        }
                        if (close(pipes[j][WRITE_END]) == -1)
                        {
                            perror("close");
                            _exit(EXIT_FAILURE);
                        }
                    }
                }
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
                // first handle IO redirection
                if (p_line->stage[i].inname)
                {
                    if (v_flag >= 3)
                    {
                        fprintf(stderr,
                                "Infile: %s\n", p_line->stage[i].inname);
                    }
                    input = open(p_line->stage[i].inname, O_RDONLY);
                    if (input == -1)
                    {
                        perror(p_line->stage[i].inname);
                        _exit(EXIT_FAILURE);
                    }
                    if (dup2(input, STDIN_FILENO) == -1)
                    {
                        perror("dup2");
                        _exit(EXIT_FAILURE);
                    }
                    if (close(input) == -1)
                    {
                        perror("close");
                        _exit(EXIT_FAILURE);
                    }
                }
                if (p_line->stage[i].outname)
                {
                    if (v_flag >= 3)
                    {
                        fprintf(stderr,
                                "Outfile: %s\n", p_line->stage[i].outname);
                    }
                    // open with rw for everyone
                    output = open(p_line->stage[i].outname,
                                  O_WRONLY | O_CREAT | O_TRUNC,
                                  (S_IRUSR | S_IWUSR |
                                   S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH));
                    if (output == -1)
                    {
                        perror(p_line->stage[i].inname);
                        _exit(EXIT_FAILURE);
                    }
                    if (dup2(output, STDOUT_FILENO) == -1)
                    {
                        perror("dup2");
                        _exit(EXIT_FAILURE);
                    }
                    if (close(output) == -1)
                    {
                        perror("close");
                        _exit(EXIT_FAILURE);
                    }
                }

                if (v_flag >= 3)
                {
                    fprintf(stderr, "Now executing %s.\n",
                            p_line->stage[i].argv[0]);
                }

                // time for execution
                execvp(p_line->stage[i].argv[0], p_line->stage[i].argv);
                perror(p_line->stage[i].argv[0]);
                _exit(EXIT_FAILURE);
            }
        }

        // unblock the parent
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

        // Parent
        // clean up pipes
        for (j = 0; j < stages - 1; j++)
        {
            // Close all pipe remaining pipe fds
            if (close(pipes[j][READ_END]) == -1)
            {
                perror("close");
                exit(EXIT_FAILURE);
            }
            if (close(pipes[j][WRITE_END]) == -1)
            {
                perror("close");
                exit(EXIT_FAILURE);
            }
        }
        // wait for all children
        for (i = 0; i < stages; i++)
        {
            // wait for our children
            if (waitpid(children[i], &status, 0) == -1)
            {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }

            //  verbose
            if (v_flag >= 3)
            {
                if (WIFEXITED(status))
                {
                    if (WEXITSTATUS(status) == 0)
                    {
                        // it did good
                        fprintf(stderr,
                                "Process %d succeeded.\n", children[i]);
                    }
                    else
                    {
                        // it did no good
                        fprintf(stderr,
                                "Process %d exited with an error value.\n",
                                children[i]);
                    }
                }
                else
                {
                    // child went abnormally
                    fprintf(stderr, "Process %d ended abnormally.\n",
                            children[i]);
                }
            }
        }

        // TO DO free pipeline and command line string (pointer to it is in
        // stored in pipeline)
        free(p_line->cline);
        free_pipeline(p_line);
        // reset stages
        stages = 0;
    }
    if (optind < argc - 1)
    {
        if (fclose(input) == -1)
        {
            perror("fclose");
            exit(EXIT_SUCCESS);
            ;
        }
    }

    return 0;
}