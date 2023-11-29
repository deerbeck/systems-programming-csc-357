#include "utility.h"

/* initialize global variables*/
int v_flag = 0;
int a_flag = 0;
int N_flag = 0;
/* sigint handler*/
void sigintHandler(int signum)
{
    /* not really needed because we are exiting anyway*/
    stop_windowing();
    exit(EXIT_SUCCESS); /* Terminate the program gracefully*/
}
/* client chat function*/
void chat(int sockfd)
{
    int done, len, mlen;
    char buff[BUFFER_SIZE];
    struct pollfd fds[REMOTE + 1];
    fds[LOCAL].fd = STDIN_FILENO;
    fds[LOCAL].events = POLLIN;
    fds[LOCAL].revents = 0;
    fds[REMOTE] = fds[LOCAL];
    fds[REMOTE].fd = sockfd;
    done = 0;
    /* activate verbosity*/
    if (v_flag)
    {
        set_verbosity(1);
    }
    /* start the chat window*/
    start_windowing();

    /* install SIGINT handler to catch that ^C*/
    signal(SIGINT, sigintHandler);

    /* go into the chat loop*/
    do
    {
        /* poll to wait for input from anywhere*/
        /* negative–>wait forever */
        if (poll(fds, sizeof(fds) / sizeof(struct pollfd), -1) == -1)
        {
            perror("poll");
            exit(EXIT_FAILURE);
        }
        /* check EOF to end if it is still in buffer*/
        if (has_hit_eof())
        {
            close(sockfd);
            exit(EXIT_SUCCESS);
        }
        /* input is doing something*/
        if (fds[LOCAL].revents & POLLIN)
        {
            /* check if line is ready to read or if we can start pulling*/
            /* first update input buffer*/
            update_input_buffer();
            /* if line is ready to read -> read it*/
            /* message is sent if either whole line or EOF is detected*/
            if (has_whole_line())
            {
                len = read_from_input(buff, BUFFER_SIZE);
                if (len == ERR)
                {
                    perror("read_from_input");
                    exit(EXIT_FAILURE);
                }
                else if (len == EOF)
                {
                    /* TO DO*/
                }
                else
                {
                    /* send it because there is something to send*/
                    mlen = send(sockfd, buff, len, 0);
                    if (mlen == -1)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
        /* or maybe the output is doing something*/
        if (fds[REMOTE].revents & POLLIN)
        {
            /* receive a line and print it to the windowing output*/
            mlen = recv(sockfd, buff, sizeof(buff), 0);
            if (mlen == -1)
            {
                perror("recv");
                exit(EXIT_FAILURE);
            }
            /* EOF is recieved(connection closed)*/
            else if (mlen == 0)
            {
                /* wait for ^C*/
                fprint_to_output("Connection closed. ^C to terminate.\n");

                /* pause everything to wait for SIGINT*/
                pause();
            }

            if (write_to_output(buff, mlen) == ERR)
            {
                perror("write_to_output");
                exit(EXIT_FAILURE);
            }
        }
    } while (!done);
    /* stop the ncuses windowing*/
}
