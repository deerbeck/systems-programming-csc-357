#include "utility.h"
/* making my IDE happy*/
extern int optind, optopt, opterr;

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
                    "Usage: %s [ -c ] [ -v ] [ -N ] [ hostname ] port\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind < argc)
    {
        hostname = argv[optind++];
    }

    if (optind < argc)
    {
        port = atoi(argv[optind]);
    }

    /* hostname is present -> acting as a client*/
    if (hostname)
    {
        client_connect();
    }

    /* if not we have a server*/
    else
    {
        /* initialize variables*/
        int sockfd, newsockfd;
        socklen_t clilen;
        char server_buffer[BUFFER_SIZE];
        /* set buffer to \0*/
        memset(server_buffer, '\0', BUFFER_SIZE);

        /* using sockaddr_in to work with ip4 protocoll more conveniently*/
        struct sockaddr_in serv_addr, cli_addr;
        int n;

        /* create endpoint for server to wait for connection*/
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            perror("socket");
            exit(EXIT_FAILURE);
        }

        /* preset server address struct*/
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);

        /* assing name to the socket*/
        if (bind(sockfd,
                 (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        {
            perror("bind");
            exit(EXIT_FAILURE);
        }
        /* listen to the socket and wait for connection request
         * I allow a maximum of 1 client*/
        if (listen(sockfd, MAX_CLIENTS) == -1)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        /* create new socket for client so server can keep listening*/
        clilen = sizeof(cli_addr);

        /* ask user to accept connection if -a option is not set*/
        if(a_flag)
        {
            newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        }
        else
        {
            /* ask user if he wants to accept the connection*/
            printf("Mytalk request from %s@%s. Accept (y/n)?");
            buffer()

        }





/* CONTINUE*/













        
        /* handle error on socket fd*/
        if (newsockfd == -1)
        {
            perror("ERROR on accept");
            exit(1);
        }

        
        n = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(1);
        }

        printf("Here is the message: %s\n", buffer);

        n = write(newsockfd, "I got your message", 18);
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(1);
        }

        close(newsockfd);
        close(sockfd);
        return 0;
    }

    printf("c_flag: %d\n", c_flag);
    printf("v_flag: %d\n", v_flag);
    printf("N_flag: %d\n", N_flag);
    printf("hostname: %s\n", hostname);
    printf("port: %d\n", port);

    return 0;
}