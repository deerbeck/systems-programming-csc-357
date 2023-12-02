#include "utility.h"

/* initialize global variables*/
int v_flag = 0;
int a_flag = 0;
int N_flag = 0;

/* client chat function*/
void chat(int sockfd)
{
    int len, mlen;
    char buff[BUFFER_SIZE];
    struct pollfd fds[REMOTE + 1];
    fds[LOCAL].fd = STDIN_FILENO;
    fds[LOCAL].events = POLLIN;
    fds[LOCAL].revents = 0;
    fds[REMOTE] = fds[LOCAL];
    fds[REMOTE].fd = sockfd;
    /* activate verbosity*/
    if (v_flag)
    {
        set_verbosity(1);
    }

    /* start the chat window*/
    /* only if -N flag is not set*/
    if (!N_flag)
    {
        start_windowing();
    }


    /* go into the chat loop*/
    /* infinite because we are breaking out if we want to end*/
    while (1)
    {
        /* poll to wait for input from anywhere*/
        /* negative–>wait forever */
        if (poll(fds, sizeof(fds) / sizeof(struct pollfd), -1) == -1)
        {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        /* first update input buffer*/
        update_input_buffer();
        /* check if EOF is in buffer and end if so but send everything before
         * that*/
        /* input is doing something*/
        if (fds[LOCAL].revents & POLLIN)
        {
            /* check if line is ready to read or if we can start reading*/
            /* if line is ready to read -> read it*/
            if (has_whole_line())
            {
                len = read_from_input(buff, sizeof(buff));
                if (len == ERR)
                {
                    perror("read_from_input");
                    exit(EXIT_FAILURE);
                }
                /* nul terminate anyway*/
                buff[len] = '\0';
                /* EOF is already handled with (has_hit_EOF)*/

                /* send it because there is something to send*/
                mlen = send(sockfd, buff, len, 0);
                if (mlen == -1)
                {
                    perror("send");
                    exit(EXIT_FAILURE);
                }

                /* check if we hit EOF and close if so*/
                if (has_hit_eof())
                {
                    close(sockfd);
                    stop_windowing();
                    exit(EXIT_SUCCESS);
                }
            }
        }
        /* or maybe the output is doing something*/
        if (fds[REMOTE].revents & POLLIN)
        {
            /* receive a line and print it to the windowing output*/
            /* len of buffer -1 to fit \0*/
            mlen = recv(sockfd, buff, sizeof(buff) - 1, 0);
            if (mlen == -1)
            {
                perror("recv");
                exit(EXIT_FAILURE);
            }
            /* EOF is recieved(connection closed) */
            else if (mlen == 0)
            {
                /* wait for ^C*/
                fprint_to_output("\nConnection closed. ^C to terminate.\n");

                /* pause everything to wait for SIGINT*/
                pause();
            }
            /* nul terminate recieved string*/
            buff[mlen] = '\0';

            /* write recieved message to output*/
            if (write_to_output(buff, mlen) == ERR)
            {
                perror("write_to_output");
                exit(EXIT_FAILURE);
            }
        }
    }
    /* stop the ncuses windowing*/
}
