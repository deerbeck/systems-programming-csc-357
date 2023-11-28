#ifndef utility_h
#define utility_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <errno.h>
#include "Talk/talk.h"

/* global variables for command line options*/
extern int v_flag;
extern int a_flag;
extern int N_flag;

/* important numbers*/
#define MAX_CLIENTS 2
#define BUFFER_SIZE 256

#endif