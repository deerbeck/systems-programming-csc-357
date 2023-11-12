#ifndef utility_h
#define utility_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <sys/sysmacros.h>
#include <time.h>
#include <sys/stat.h>
#include <ctype.h>
#include <utime.h>
#include <errno.h>

#define READ_WRITE_BUFFER_SIZE 4096
#define PATH_LENGTH 256
#define BLOCK_SIZE 512

/*define all the sizes of the Header-Block --->>> no mo magic numbers*/
#define NAME_SIZE 100
#define MODE_SIZE 8
#define UID_SIZE 8
#define GID_SIZE 8
#define SIZE_SIZE 12
#define MTIME_SIZE 12
#define CHKSUM_SIZE 8
#define TYPEFLAG_SIZE 1
#define LINKNAME_SIZE 100
#define MAGIC_SIZE 6
#define VERSION_SIZE 2
#define UNAME_SIZE 32
#define GNAME_SIZE 32
#define DEVMAJOR_SIZE 8
#define DEVMINOR_SIZE 8
#define PREFIX_SIZE 155
#define PADDING_SIZE 12
/* command line options to be available in every function*/
extern int verbose;
extern int create;
extern int listing;
extern int extract;
extern int strict;
extern int file;

/* For interoperability with GNU tar. GNU seems to
 * set the high–order bit of the ﬁrst byte, then
 * treat the rest of the ﬁeld as a binary integer
 * in network byte order.
 * I don’t know for sure if it’s a 32 or 64–bit int, but for
 * this version, we’ll only support 32. (well, 31)
 * returns the integer on success, –1 on failure.
 * In spite of the name of htonl(), it converts int32 t
 */
uint32_t extract_special_int(char *where, int len);

/* For interoperability with GNU tar. GNU seems to
 * set the high–order bit of the ﬁrst byte, then
 * treat the rest of the ﬁeld as a binary integer
 * in network byte order.
 * Insert the given integer into the given ﬁeld
 * using this technique. Returns 0 on success, nonzero
 * otherwise
 */
int insert_special_int(char *where, size_t size, int32_t val);

/* function to traverse directory and subdirectories and create archive*/
void archive_file(const char *pathname, int tar_filedes);

/* Header struct to store all necessary data in*/
typedef struct __attribute__((packed)) Header
{
    /* struct and size according to specification
     * Read USART Header Format for "Magic Numbers"*/
    char name[NAME_SIZE];
    char mode[MODE_SIZE];
    char uid[UID_SIZE];
    char gid[GID_SIZE];
    char size[SIZE_SIZE];
    char mtime[MTIME_SIZE];
    char chksum[CHKSUM_SIZE];
    char typeflag[TYPEFLAG_SIZE];
    char linkname[LINKNAME_SIZE];
    char magic[MAGIC_SIZE];
    char version[VERSION_SIZE];
    char uname[UNAME_SIZE];
    char gname[GNAME_SIZE];
    char devmajor[DEVMAJOR_SIZE];
    char devminor[DEVMINOR_SIZE];
    char prefix[PREFIX_SIZE];
    char padding[PADDING_SIZE];

} Header;

/* Helperfunction to calculate the ckecksum*/
unsigned int calc_checksum(Header *header_struct);

/* helperfunction to validate the ckecksum stored in the Header*/
int validate_header(Header *header_struct);

/* take the path_name and the struct to create the header struct with all
 * needed information*/
Header *create_header(const char *pathname, struct stat stat_struct);

/* helper function to extract the data stored in the header block and store in
 * header struct*/
Header *extract_header(char *header_data);

/* helper function to check if last two blocks are reached*/
int check_archive_end(int tar_filedes);

/* print out the contents of the tape archive*/
void list_tar(char **shopping_list, int tar_filedes, int num_paths);

/* Helperfunction to print out the permissions in listing mode*/
void print_permissions(mode_t mode);

/* Helperfunction to print out user name and group of file in listing mode*/
void print_owner_group(Header *header_struct);

/* Helperfunction to print out the las modification time as specified*/
void print_time(time_t time);

/* Helperfunction to print out name of file*/
void build_name(Header *header_struct, char *full_name);

/* Helperfunction to bundle all print statements*/
void print_header_info(Header *header_struct, char *full_name);

/* Little helper function to remove '/' from directory pathname*/
void stripLastCharacter(char *str);

/* extract files from archive*/
void extract_archive(char **shopping_list, int tar_filedes, int num_paths);

/* Helper function to set the modification time*/
void set_mod_time(char *full_name, time_t mod_time);

int make_nested_directory(char *full_name);
#endif