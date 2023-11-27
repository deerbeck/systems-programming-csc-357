#ifndef create_util_h
#define create_util_h

#include "utility.h"


/* function to traverse directory and subdirectories and create archive*/
void archive_file(const char *pathname, int tar_filedes);

/* take the path_name and the struct to create the header struct with all
 * needed information*/
Header *create_header(const char *pathname, struct stat stat_struct);

#endif