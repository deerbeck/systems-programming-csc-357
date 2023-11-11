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

#define READ_WRITE_BUFFER_SIZE 4096
#define PATH_LENGTH 256
#define BLOCK_SIZE 512

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
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag[1];
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];

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
void list_tar(const char *pathname, int tar_filedes);

/* Helperfunction to print out the permissions in listing mode*/
void print_permissions(mode_t mode);

/* Helperfunction to print out user name and group of file in listing mode*/
void print_owner_group(Header *header_struct);
/* Helperfunction to print out the las modification time as specified*/
void print_time(time_t time);
/* Helperfunction to print out name of file*/
void build_name(Header *header_struct, char *full_name);
#endif