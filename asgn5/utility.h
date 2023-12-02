#ifndef utility_h
#define utility_h

#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <poll.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pwd.h>
#include <poll.h>
#include <signal.h>

#include "Talk/talk.h"

#include <talk.h>

/* global variables for command line options*/
extern int v_flag;
extern int a_flag;
extern int N_flag;

/* important numbers*/
#define MAX_BACKLOG 10
#define BUFFER_SIZE 1000

#define LOCAL 0
#define REMOTE (LOCAL + 1)

/* Helper function to outsource the chat functionality*/
void chat(int sockfd);

#endif