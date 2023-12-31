#include "utility.h"

extern int optind, optopt, opterr;
extern int h_errno;

int main(int argc, char *argv[])
{
    char *hostname = NULL;
    int port = 0;

    /* handle usage of program*/
    if (argc < 2)
    {
        fprintf(stderr,
                "Usage: %s [ -v ] [ -a ] [ -N ] [ hostname ] port\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    /* parsing command line options*/
    int opt;
    while ((opt = getopt(argc, argv, "vaN")) != -1)
    {
        switch (opt)
        {
        case 'v':
            v_flag = 1;
            break;
        case 'a':
            a_flag = 1;
            break;
        case 'N':
            N_flag = 1;
            break;
        default: /* '?' */
            fprintf(stderr,
                    "Usage: %s [ -v ] [ -a ] [ -N ] [ hostname ] port\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    /* Getting the hostname and port*/
    if (optind < argc - 1)
    {
        hostname = argv[optind];
        port = atoi(argv[optind + 1]);
    }
    else if (optind == argc - 1)
    {
        port = atoi(argv[optind]);
    }
    else
    {
        fprintf(stderr, "Usage: %s [-v] [-a] [-N] [hostname] port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /*########### CLIENT ###########*/
    if (hostname)
    {
        /* initialize variablse*/
        int sockfd;
        struct sockaddr_in serv_addr;
        struct hostent *hostent;
        ssize_t client_message_len;
        char client_buffer[BUFFER_SIZE];

        /* who are we talking to*/
        hostent = gethostbyname(hostname);
        if (!hostent)
        {
            perror("gethostbyname");
            exit(EXIT_FAILURE);
        }
        /* create endpoint for connection*/
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        /* try to establishsh connection*/
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        serv_addr.sin_addr.s_addr = *(uint32_t *)hostent->h_addr_list[0];

        if (connect(sockfd, (struct sockaddr *)&serv_addr,
                    sizeof(serv_addr)) == -1)
        {
            perror("connect");
            exit(EXIT_FAILURE);
        }

        /* we are hopefully connected*/
        /* waiting to connect*/
        printf("Waiting for response from %s.\n", hostent->h_name);
        /* gather username from process*/
        struct passwd *pw_struct;
        if (!(pw_struct = getpwuid(getuid())))

        {
            perror("getpwuid");
            exit(EXIT_FAILURE);
        }

        /* send out username to host*/
        snprintf(client_buffer, BUFFER_SIZE, "%s", pw_struct->pw_name);
        /* len plus one to include '\0' character*/
        if (send(sockfd,
                 client_buffer, strlen(client_buffer) + 1, 0) == -1)
        {
            perror("send");
            exit(EXIT_FAILURE);
        }

        /* recieve response from host*/
        /* len of buffer -1 to fit \0*/
        client_message_len = recv(sockfd,
                                  client_buffer,
                                  sizeof(client_buffer) - 1, 0);
        if ((client_message_len) == -1)
        {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        /* put Nul character */
        client_buffer[client_message_len] = '\0';

        /* if not ok host declined connection*/
        if (strcmp(client_buffer, "ok"))
        {
            printf("%s declined connection.\n", hostent->h_name);
            if (close(sockfd) == -1)
            {
                perror("close");
                exit(EXIT_FAILURE);
            }
            return 0;
        }

        /* else we can chat*/
        chat(sockfd);
        /* closing not really needed because exit takes care of it*/
        close(sockfd);
    }

    /*########### SERVER ###########*/
    /* if not we have a server*/
    else
    {
        /* initialize variables*/
        int sockfd, newsockfd;
        socklen_t len;
        char server_buffer[BUFFER_SIZE];
        ssize_t server_message_len;

        /* using sockaddr_in to work with ip4 protocoll more conveniently*/
        struct sockaddr_in serv_addr, cli_info, peer_info;
        char cli_addr[INET_ADDRSTRLEN], peer_addr[INET_ADDRSTRLEN];

        /* create endpoint for server to wait for connection*/
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        /* preset server address struct*/
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        /* bind socket*/
        if (bind(sockfd,
                 (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        {
            perror("bind");
            exit(EXIT_FAILURE);
        }

        /* listen to the socket and wait for connection request*/
        if (listen(sockfd, MAX_BACKLOG) == -1)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        /* verbose stuff*/
        if(v_flag)
        {
            printf("Waiting for connection...\n");
        }

        /* get new socket for client so server can keep listening*/
        len = sizeof(peer_info);
        /* ask user to accept connection if -a option is not set*/
        newsockfd = accept(sockfd, (struct sockaddr *)&peer_info, &len);

        if (newsockfd == -1)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        /* skip if a_flag is set*/
        /* get info about client*/
        len = sizeof(cli_info);
        if (getsockname(newsockfd, (struct sockaddr *)&cli_info, &len) == -1)
        {
            perror("getsockname");
            exit(EXIT_FAILURE);
        }

        if (v_flag)
        {
            /*get info about new local socket and connected peer*/
            inet_ntop(AF_INET, &cli_info.sin_addr.s_addr,
                      cli_addr, sizeof(cli_addr));
            inet_ntop(AF_INET, &peer_info.sin_addr.s_addr,
                      peer_addr, sizeof(peer_addr));
            printf("New Connection: %s:%d->%s:%d\n",
                   peer_addr, ntohs(peer_info.sin_port),
                   cli_addr, ntohs(cli_info.sin_port));
        }

        /* recieve username from client*/
        /* len of buffer -1 to fit \0*/
        server_message_len = recv(newsockfd,
                                  server_buffer,
                                  sizeof(server_buffer) - 1, 0);

        if (server_message_len == -1)
        {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        /* null terminate message*/
        server_buffer[server_message_len] = '\0';

        /*debugging*/
        if (v_flag)
        {
            printf("Peer username: %s\n", server_buffer);
        }

        /* get hostname of peer*/
        struct sockaddr_storage addr;
        socklen_t addr_len = sizeof(addr);
        char host[NI_MAXHOST], service[NI_MAXSERV];

        if (getpeername(newsockfd,
                        (struct sockaddr *)&addr, &addr_len) == -1)
        {
            perror("getpeername");
            exit(EXIT_FAILURE);
        }
        if (getnameinfo((struct sockaddr *)&addr, addr_len, host,
                        NI_MAXHOST, service, NI_MAXSERV, 0))
        {
            perror("getnameinfo");
            exit(EXIT_FAILURE);
        }

        else
        {
            if (!a_flag)
            {
                /* ask user if he wants to accept the connection*/
                printf("Mytalk request from %s@%s. Accept (y/n)? ",
                       server_buffer, host);
                memset(server_buffer, '\0', BUFFER_SIZE);

                /* get answer from server user*/
                scanf("%s", server_buffer);

                /* check if yes or y and send ok or not ok package*/
                if (!(strcasecmp(server_buffer, "yes") &&
                      strcasecmp(server_buffer, "y")))
                {
                    /* len plus one to include '\0' character*/
                    if (send(newsockfd, "ok",
                             strlen("ok") + 1, 0) == -1)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                }
                /* connection not accepted*/
                else
                {
                    /* len plus one to include '\0' character*/
                    if (send(newsockfd, "not ok",
                             strlen("not ok") + 1, 0) == -1)
                    {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                    /* throw away connection*/
                    if (close(newsockfd) == -1)
                    {
                        perror("close");
                        exit(EXIT_FAILURE);
                    }
                    /* end program*/
                    exit(EXIT_SUCCESS);
                }
            }
            /* else --> don't ask*/
            else
            {
                /* len plus one to include '\0' character*/
                if (send(newsockfd, "ok",
                         strlen("ok") + 1, 0) == -1)
                {
                    perror("send");
                    exit(EXIT_FAILURE);
                }
            }
        }

        /* now chat! */
        chat(newsockfd);

        /* close connection*/;
        if (close(newsockfd) == -1)
        {
            perror("close");
            exit(EXIT_FAILURE);
        }
        if (close(sockfd) == -1)
        {
            perror("close");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}