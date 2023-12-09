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
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "Mush/mush.h"

#include <mush.h>

#define READ_END 0
#define WRITE_END 1

// used for debug
#define MAX_FD 1024

// global variables
extern int v_flag;
extern int b_processing;

// Signal handler function for SIGINT - Batch
void sigintHandlerBatch(int sig_num);

// Signal handler function for SIGINT - Child
void sigintHandlerInteractive(int sig_num);

// cleanup located in one function to make it look nicer
void cleanup(sigset_t old_mask, pipeline p_line);

// for debug purposes
void print_open_fds(void);

// handle a command (parse it)
pipeline get_command(FILE *input);

#endif