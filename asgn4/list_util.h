#ifndef list_util_h
#define list_util_h

#include "utility.h"

/* print out the contents of the tape archive*/
void list_tar(char **shopping_list, int tar_filedes, int num_paths);

/* Helperfunction to print out the permissions in listing mode*/
void print_permissions(mode_t mode);

/* Helperfunction to print out user name and group of file in listing mode*/
void print_owner_group(Header *header_struct);

/* Helperfunction to print out the las modification time as specified*/
void print_time(time_t time);

/* Helperfunction to bundle all print statements*/
void print_header_info(Header *header_struct, char *full_name);



#endif