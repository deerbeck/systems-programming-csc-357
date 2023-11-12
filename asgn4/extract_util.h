#ifndef extract_util_h
#define extract_util_h

#include "utility.h"

/* Little helper function to remove '/' from directory pathname*/
void stripLastCharacter(char *str);

/* extract files from archive*/
void extract_archive(char **shopping_list, int tar_filedes, int num_paths);

/* Helper function to set the modification time*/
void set_mod_time(char *full_name, time_t mod_time);

/* Helper function to create nested and normal directories if not already
existing*/
int make_nested_directory(char *full_name);

#endif
