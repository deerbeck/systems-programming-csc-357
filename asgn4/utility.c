#include "utility.h"

/* initialize global variables*/
int verbose = 0;
int create = 0;
int listing = 0;
int extract = 0;
int strict = 0;
int file = 0;

uint32_t extract_special_int(char *where, int len)
{
    int32_t val = -1;
    if ((len >= sizeof(val)) && (where[0] & 0x80))
    {
        /* the top bit is set and we have space
         * extract the last four bytes */
        val = *(int32_t *)(where + len - sizeof(val));
        val = ntohl(val);
        /* convert to host byte order */
    }
    return val;
}

int insert_special_int(char *where, size_t size, int32_t val)
{

    int err = 0;
    if (val < 0 || (size < sizeof(val)))
    {
        /* if it’s negative, bit 31 is set and we can’t use the ﬂag
         * if len is too small, we can’t write it. Either way, we’re
         * done.
         */
        err++;
    }
    else
    {
        /* game on....*/
        memset(where, 0, size);
        /* Clear out the buﬀer */
        *(int32_t *)(where + size - sizeof(val)) = htonl(val);
        /* place the int */
        *where |= 0x80;
        /* set that high–order bit */
    }
    return err;
}

void archive_file(const char *pathname, int tar_filedes)
{
    /* create buffer for absolute paths*/
    char path_buffer[PATH_LENGTH];
    strcpy(path_buffer, pathname);

    /* initialize direntry and dir*/
    DIR *current_dir;
    struct dirent *current_entry;

    struct stat stat_buf;
    /* if stat's couldn't be used on file, return and handle next path*/
    if (lstat(path_buffer, &stat_buf) == -1)
    {
        fprintf(stderr, "Couldn't stat %s.\n", path_buffer);
        return;
    }

    /* if verbose is turned on print out path*/
    if (verbose)
    {
        printf("%s\n", path_buffer);
    }

    /* write file to archive*/
    Header *header_struct = create_header(path_buffer, stat_buf);

    /* if null continue to next file*/
    if (!header_struct)
    {
        fprintf(stderr, "Skipping...\n");
        return;
    }
    /* write header to data*/
    /* because of packing attribute i can directly write the struct*/
    write(tar_filedes, header_struct, sizeof(Header));

    /* free header because of no more use while writing*/
    free(header_struct);

    /* now onto the contents of regular files file*/
    if (S_ISREG(stat_buf.st_mode))
    {
        int data_filedes = open(path_buffer, O_RDONLY);
        if (data_filedes == -1)
        {
            fprintf(stderr, "%s could not be opened. Skipping...", path_buffer);
            return;
        }

        ssize_t bytes_read;
        char write_buffer[BLOCK_SIZE];
        /*set buffer to 0 this takes care of the 0 padding*/
        int i;
        for (i = 0; i < BLOCK_SIZE; i++)
        {
            write_buffer[i] = '\0';
        }
        while ((bytes_read =
                    read(data_filedes, write_buffer, BLOCK_SIZE)) != 0)
        {
            if (bytes_read == -1)
            {
                fprintf(stderr, "Read error. Skipping...");
                return;
            }
            if (write(tar_filedes, write_buffer, BLOCK_SIZE) == -1)
            {
                fprintf(stderr, "Write error. Skipping...");
                return;
            }
            /*reset buffer this takes care of the 0 padding*/
            for (i = 0; i < BLOCK_SIZE; i++)
            {
                write_buffer[i] = '\0';
            }
        }
    }
    /* check if direcotry and traverse down accordingly*/
    if (S_ISDIR(stat_buf.st_mode))
    {
        /* open directory to get its entries*/
        current_dir = opendir(path_buffer);
        if (!current_dir)
        {
            perror("directory currupted");
            exit(EXIT_FAILURE);
        }

        /*loop through directory entries and archive each*/
        while ((current_entry = readdir(current_dir)) != NULL)
        {
            /* check for "." and ".." entry and skip*/
            if (!strcmp(current_entry->d_name, "."))
                continue;
            else if (!strcmp(current_entry->d_name, ".."))
                continue;
            /* if other continue traversing*/
            else
            {
                /* create new pathname and traverse down*/
                strcat(path_buffer, "/");
                strcat(path_buffer, current_entry->d_name);

                /* now archive found file*/
                archive_file(path_buffer, tar_filedes);

                /* reset pathname*/
                strcpy(path_buffer, pathname);
            }
        }
    }
}

unsigned int calc_checksum(Header *header_struct)
{
    /* all the numbers used are taken from the header specification*/
    unsigned int checksum = 0;
    int i = 0;
    for (i = 0; i < 100; i++)
    {
        checksum += (unsigned char)header_struct->name[i];
    }
    for (i = 0; i < 8; i++)
    {
        checksum += (unsigned char)header_struct->mode[i];
    }
    for (i = 0; i < 8; i++)
    {
        checksum += (unsigned char)header_struct->uid[i];
    }
    for (i = 0; i < 8; i++)
    {
        checksum += (unsigned char)header_struct->gid[i];
    }
    for (i = 0; i < 12; i++)
    {
        checksum += (unsigned char)header_struct->size[i];
    }
    for (i = 0; i < 12; i++)
    {
        checksum += (unsigned char)header_struct->mtime[i];
    }
    for (i = 0; i < 8; i++)
    {
        checksum += (unsigned char)' ';
    }
    for (i = 0; i < 1; i++)
    {
        checksum += (unsigned char)header_struct->typeflag[i];
    }
    for (i = 0; i < 100; i++)
    {
        checksum += (unsigned char)header_struct->linkname[i];
    }
    for (i = 0; i < 6; i++)
    {
        checksum += (unsigned char)header_struct->magic[i];
    }
    for (i = 0; i < 2; i++)
    {
        checksum += (unsigned char)header_struct->version[i];
    }
    for (i = 0; i < 32; i++)
    {
        checksum += (unsigned char)header_struct->uname[i];
    }
    for (i = 0; i < 32; i++)
    {
        checksum += (unsigned char)header_struct->gname[i];
    }
    for (i = 0; i < 8; i++)
    {
        checksum += (unsigned char)header_struct->devmajor[i];
    }
    for (i = 0; i < 8; i++)
    {
        checksum += (unsigned char)header_struct->devminor[i];
    }
    for (i = 0; i < 155; i++)
    {
        checksum += (unsigned char)header_struct->prefix[i];
    }
    return checksum;
}

int validate_header(Header *header_struct)
{
    unsigned int chcksum_calc = calc_checksum(header_struct);
    unsigned int chcksum_stored = strtol(header_struct->chksum, NULL, 8);
    char magic_buf[6];
    int i;
    for (i = 0; i < 6; i++)
    {
        magic_buf[i] = tolower(header_struct->magic[i]);
    }

    if (chcksum_calc != chcksum_stored)
    {
        fprintf(stderr, "Checksum not valid.\n");
        return 0;
    }
    /*  else if (strcmp(magic_buf, "ustar"))
     {
         fprintf(stderr, "Magic number not valid.\n");
         return 0;
     }
     else if (header_struct->version[0] != '0' ||
              header_struct->version[1] != '0')
     {
         fprintf(stderr, "Version not valid.\n");
         return 0;
     } */
    else
        return 1;
}

Header *create_header(const char *pathname, struct stat stat_struct)
{
    /* all struct attributes and their properties can be observed in the USART
    Archive format - Header Format specification*/
    /* use calloc instead of malloc to set all to \0 and handle padding*/
    Header *header_struct = (Header *)calloc(sizeof(Header), sizeof(char));
    char path_buffer[PATH_LENGTH];
    strcpy(path_buffer, pathname);

    if (!header_struct)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    /* checksum*/
    unsigned int checksum;

    /* first of all, handle path name -> checking length and split to fit into
    name and prefix*/

    if (strlen(path_buffer) > 100)
    {
        char *split_pos = path_buffer;
        char *end_path = strchr(path_buffer, '\0');

        split_pos = strchr(split_pos, '/');
        while (end_path - split_pos > 100)
        {
            split_pos = strchr(split_pos + 1, '/');

            /* name can not be split*/
            if (!split_pos)
            {
                fprintf(stderr, "%s couldn't be split at '/'\n", path_buffer);
                return NULL;
            }
        }

        /* now copy end of filename to name in header (split_pos+1 to exclude
        '/')*/
        strncpy(header_struct->name, split_pos + 1, 100);

        /* put rest of name in prefix*/
        strncpy(header_struct->prefix, path_buffer, split_pos - path_buffer);
    }
    else
    {
        strncpy(header_struct->name, path_buffer, 100);
    }
    /* add '/' if directory*/
    if (S_ISDIR(stat_struct.st_mode) && strlen(header_struct->name) < 100)
    {
        strcat(header_struct->name, "/");
    }

    /* saving the mode print into mode array and strip of filetype*/
    sprintf(header_struct->mode, "%07o", (stat_struct.st_mode & 07777));

    /* next up is the uid and gid of the file*/
    sprintf(header_struct->uid, "%07o", (unsigned int)stat_struct.st_uid);
    sprintf(header_struct->gid, "%07o", (unsigned int)stat_struct.st_gid);

    /* size of regular file*/
    if (S_ISREG(stat_struct.st_mode))
    {
        sprintf(header_struct->size, "%011o",
                (unsigned int)stat_struct.st_size);
    }
    /* for DIR and SYMLINNK size is 0*/
    else
    {
        sprintf(header_struct->size, "%011o",
                (unsigned int)0);
    }
    /* last modified time of file*/
    sprintf(header_struct->mtime, "%011o",
            (unsigned int)stat_struct.st_mtime);

    /* checksum will be handled later!!*/

    /* store typeflag*/
    if (S_ISREG(stat_struct.st_mode))
        header_struct->typeflag[0] = '0';
    else if (S_ISLNK(stat_struct.st_mode))
        header_struct->typeflag[0] = '2';
    else if (S_ISDIR(stat_struct.st_mode))
        header_struct->typeflag[0] = '5';

    /* store linkname*/
    if (S_ISLNK(stat_struct.st_mode))
    {
        if (readlink(path_buffer, header_struct->linkname, 100) == -1)
        {
            fprintf(stderr, "Can not read link. Skipping...");
        }
    }

    /* store magic "ustar"*/
    strncpy(header_struct->magic, "ustar", 6);

    /* store '00' version*/
    header_struct->version[0] = '0';
    header_struct->version[1] = '0';

    /* get username and store in header*/
    struct passwd *pwd = getpwuid(stat_struct.st_uid);
    if (!pwd)
    {
        fprintf(stderr, "Username couldn't be fetched.\n");
    }
    else
    {
        strncpy(header_struct->uname, pwd->pw_name, 32);
    }

    /* get group name and store in header*/
    struct group *grp = getgrgid(stat_struct.st_gid);
    if (!grp)
    {
        fprintf(stderr, "Groupname couldn't be fetched.\n");
    }
    else
    {
        strncpy(header_struct->gname, grp->gr_name, 32);
    }

    /* get major devnumber*/
    sprintf(header_struct->devmajor, "%07o",
            (unsigned int)major(stat_struct.st_dev));
    /* get minor devnumber*/
    sprintf(header_struct->devminor, "%07o",
            (unsigned int)minor(stat_struct.st_dev));

    /* finaly calculate the checksum*/
    checksum = calc_checksum(header_struct);
    /* finaly write checksum*/
    sprintf(header_struct->chksum, "%07o", checksum);

    return header_struct;
}

Header *extract_header(char *header_data)
{
    Header *header_struct = (Header *)calloc(sizeof(Header), sizeof(char));
    if (!header_struct)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    memcpy(header_struct, header_data, BLOCK_SIZE);
    return header_struct;
}

int check_archive_end(int tar_filedes)
{
    off_t current_offset = lseek(tar_filedes, 0, SEEK_CUR);
    char buffer[2 * BLOCK_SIZE];
    if (read(tar_filedes, buffer, 2 * BLOCK_SIZE) == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }
    int i;
    /* check if last two blocks are reached*/
    for (i = 0; i < 2 * BLOCK_SIZE; i++)
    {
        if (buffer[i] != '\0')
        {
            /* reset offset*/
            lseek(tar_filedes, current_offset, SEEK_SET);
            return 0;
        }
    }
    return 1;
}

void list_tar(const char *pathname, int tar_filedes)
{

    Header *header_struct;
    ssize_t bytes_read;
    char header_data[BLOCK_SIZE];
    int num_data_blocks;

    char path_buffer[PATH_LENGTH];
    strcpy(path_buffer, pathname);

    char full_name[PATH_LENGTH];

    while ((bytes_read = read(tar_filedes, header_data, BLOCK_SIZE)) != 0)
    {
        /* reset name*/
        memset(full_name, '\0', PATH_LENGTH);

        if (bytes_read == -1)
        {
            perror("Reading file");
            exit(EXIT_FAILURE);
        }
        /* get header struct out of read data*/
        header_struct = extract_header(header_data);
        if (!header_struct)
        {
            fprintf(stderr, "Error extracting Header Data. Skipping...\n");
            continue;
        }
        if (!validate_header(header_struct))
        {
            fprintf(stderr, "Header not valid. I give up...\n");
            exit(EXIT_FAILURE);
        }

        /* extract number of data blocks to skip to read next header
         * (header_struct->size +BLOCK_SIZE -1) / BLOCK_SIZE round up to next
         * integer*/
        /* 8 is base used for octal conversion*/
        num_data_blocks =
            (strtol(header_struct->size, NULL, 8) + BLOCK_SIZE - 1) /
            BLOCK_SIZE;

        /* seek file to next header location*/
        lseek(tar_filedes, num_data_blocks * BLOCK_SIZE, SEEK_CUR);
        /* get full name to check for pathname and for later printing*/
        build_name(header_struct, full_name);

        if (strstr(full_name, path_buffer))
        {
            if (verbose)
            {
                print_header_info(header_struct, full_name);
            }
            /* just print name in non-verbose*/
            else
            {
                printf("%s\n", full_name);
            }
        }
        /* free header_struct*/
        free(header_struct);
        /* check if end of archive is reached and end*/
        if (check_archive_end(tar_filedes))
        {
            break;
        }
    }

    /* seek back to start to seek more paths*/
    lseek(tar_filedes, 0, SEEK_SET);
}

void print_permissions(mode_t mode)
{
    /* print out permissions to stdout*/
    putchar((mode & S_IRUSR) ? 'r' : '-');
    putchar((mode & S_IWUSR) ? 'w' : '-');
    putchar((mode & S_IXUSR) ? 'x' : '-');
    putchar((mode & S_IRGRP) ? 'r' : '-');
    putchar((mode & S_IWGRP) ? 'w' : '-');
    putchar((mode & S_IXGRP) ? 'x' : '-');
    putchar((mode & S_IROTH) ? 'r' : '-');
    putchar((mode & S_IWOTH) ? 'w' : '-');
    putchar((mode & S_IXOTH) ? 'x' : '-');
}

void print_owner_group(Header *header_struct)
{
    /* buffer strings to store user and owner in*/
    /* 8/17 is length from specification but we need 1 more for each to account
    for \0 character*/
    char owner[9];
    char group[9];
    char result[18];
    int i;
    /* I like clean data*/
    /* can do with memset aswell*/
    for (i = 0; i < 9; i++)
    {
        owner[i] = '\0';
        group[i] = '\0';
    }
    for (i = 0; i < 18; i++)
    {
        result[i] = '\0';
    }

    if (strlen(header_struct->uname) > 0)
    {
        strncpy(owner, header_struct->uname, 8);
    }
    else
    {
        sprintf(owner, "%7d", (int)strtol(header_struct->uid, NULL, 8));
    }
    /* now get group name*/
    if (strlen(header_struct->gname) > 0)
    {
        strncpy(group, header_struct->gname, 8);
    }
    else
    {
        sprintf(group, "%7d", (int)strtol(header_struct->gid, NULL, 8));
    }
    /* copy to result and print out in the end*/
    strcpy(result, owner);
    strcat(result, "/");
    strcat(result, group);
    printf("%-17s", result);
}

void print_time(time_t time)
{
    // Convert the time to a struct tm in local time
    struct tm *localTime = localtime(&time);

    // Check if localtime failed
    if (localTime == NULL)
    {
        perror("Error getting local time");
    }

    // Format the time as a string
    char formattedTime[20]; // YYYY-MM-DD HH:MM\0
    strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M", localTime);

    // Print the formatted time
    printf("%s", formattedTime);
}

void build_name(Header *header_struct, char *full_name)
{
    /* numbers taken from specification +1 for \0 termination*/
    char prefix[156];
    char name[101];

    memcpy(name, header_struct->name, 101);
    /* NUL Terminate for printf to work*/
    name[100] = '\0';
    /* print out name of file*/
    /* include prefix if applicable*/
    if (header_struct->prefix[0] != '\0')
    {
        memcpy(prefix, header_struct->prefix, 156);
        /* NUL Terminate for printf to work*/
        prefix[155] = '\0';
        sprintf(full_name, "%s/%s", prefix, name);
    }
    else
    {
        /* handle if directory barely fits in name buffer*/
        if (header_struct->typeflag[0] == '5' && strlen(name) == 100)
        {
            sprintf(full_name, "%s/", name);
        }
        else
        {
            sprintf(full_name, "%s", name);
        }
    }
}

void print_header_info(Header *header_struct, char *full_name)
{
    /* switch case to print out the type of files*/
    switch (header_struct->typeflag[0])
    {
    case '0':
    case '\0':
        putchar('-');
        break;

    case '2':
        putchar('l');
        break;

    case '5':
        putchar('d');
        break;

    default:
        fprintf(stderr, "Filetype nto supported");
        break;
    }
    /* pass mode to helper function 8 is base for octal*/
    print_permissions((mode_t)strtol(header_struct->mode, NULL, 8));
    /* put whitespace*/
    putchar(' ');
    /* print out owner / if not available the uid*/
    print_owner_group(header_struct);
    /* put whitespace*/
    putchar(' ');
    /*print out size of file*/
    printf("%7d", (int)strtol(header_struct->size, NULL, 8));
    /* put whitespace*/
    putchar(' ');
    /* print out modification time*/
    print_time((time_t)strtol(header_struct->mtime, NULL, 8));
    /* put whitespace*/
    putchar(' ');

    /* print out name of file*/
    printf("%s\n", full_name);
}

void stripLastCharacter(char *str)
{
    size_t len = strlen(str);
    if (len > 0)
    {
        str[len - 1] = '\0';
    }
}

void extract_archive(const char *pathname, int tar_filedes)
{
    Header *header_struct;
    ssize_t bytes_read;
    ssize_t bytes_written;

    char header_data[BLOCK_SIZE];
    char file_data[BLOCK_SIZE];
    int num_data_blocks;
    int last_block_data_size;

    char path_buffer[PATH_LENGTH];
    strcpy(path_buffer, pathname);

    char full_name[PATH_LENGTH];

    int i;

    while ((bytes_read = read(tar_filedes, header_data, BLOCK_SIZE)) != 0)
    {
        /* reset name*/
        memset(full_name, '\0', PATH_LENGTH);

        if (bytes_read == -1)
        {
            perror("Reading file");
            exit(EXIT_FAILURE);
        }
        /* get header struct out of read data*/
        header_struct = extract_header(header_data);
        if (!header_struct)
        {
            fprintf(stderr, "Error extracting Header Data. Skipping...\n");
            continue;
        }
        if (!validate_header(header_struct))
        {
            fprintf(stderr, "Header not valid. I give up...\n");
            exit(EXIT_FAILURE);
        }

        /* get full name to check for pathname and for later printing*/
        build_name(header_struct, full_name);

        /* extract if pathname given*/
        if (strstr(full_name, path_buffer))
        {
            if (verbose)
            {
                printf("%s\n", full_name);
            }

            /* first check if directory and creat it*/
            if (header_struct->typeflag[0] == '5')
            {
                stripLastCharacter(full_name);
                if (make_nested_directory(full_name) == -1)
                {
                    fprintf(stderr, "Couldn't create directory\n");
                    /* don't need to skip data because directory data size =
                     * 0*/
                    continue;
                }
            }
            /* now to the creation of regular files*/
            else if (header_struct->typeflag[0] == '0' ||
                     header_struct->typeflag[0] == '\0')
            {
                /* create all directories needed for file*/
                char parent_path[PATH_LENGTH];
                memset(parent_path, '\0', PATH_LENGTH);
                /* check if file is in subdirs and create them accordingly*/
                char *file_start = strrchr(full_name, '/');
                if (
                    file_start)
                {
                    strncpy(parent_path, full_name,
                            (file_start - full_name));

                    if (make_nested_directory(parent_path) == -1)
                    {
                        fprintf(stderr, "Couldn't create directory\n");
                    }
                }

                /* get correct permissions for file rw for everyone and execute
                 * if anyone has execute*/
                mode_t file_mode = 0;
                if ((mode_t)strtol(header_struct->mode, NULL, 8) &
                    (S_IXUSR | S_IXGRP | S_IXOTH))
                {
                    file_mode = (S_IRWXU | S_IRWXG | S_IRWXO);
                }
                else
                {
                    file_mode = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP |
                                 S_IROTH | S_IWOTH);
                }
                /* create file to store data in*/
                int file_fildes;
                file_fildes = open(full_name, O_WRONLY | O_CREAT | O_TRUNC,
                                   file_mode);
                if (file_fildes == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }

                /* extract number of data blocks to know how much to read
                 * (header_struct->size +BLOCK_SIZE -1) / BLOCK_SIZE round up
                 * to next integer*/
                /* 8 is base used for octal conversion*/

                num_data_blocks =
                    (strtol(header_struct->size, NULL, 8) + BLOCK_SIZE - 1) /
                    BLOCK_SIZE;
                /* get how much to write if the last block is not completly
                 * filled*/
                last_block_data_size = strtol(header_struct->size, NULL, 8) %
                                       BLOCK_SIZE;
                for (i = 0; i < num_data_blocks; i++)
                {
                    bytes_read = read(tar_filedes, file_data,
                                      BLOCK_SIZE);
                    if (bytes_read == -1)
                    {
                        perror("read");
                        exit(EXIT_FAILURE);
                    }
                    /* handle last data block*/
                    if (i == num_data_blocks - 1)
                    {

                        bytes_written = write(file_fildes, file_data,
                                              last_block_data_size);
                        if (bytes_written == -1)
                        {
                            perror("write");
                            exit(EXIT_FAILURE);
                        }
                    }
                    else
                    {
                        bytes_written = write(file_fildes, file_data,
                                              bytes_read);
                        if (bytes_written == -1)
                        {
                            perror("write");
                            exit(EXIT_FAILURE);
                        }
                    }
                }

                /* close file*/
                close(file_fildes);
                /* finaly set the modifaction time of the created file*/
                set_mod_time(full_name,
                             (time_t)strtol(header_struct->mtime, NULL, 8));
            }
            /* now handle symlinks*/
            else if (header_struct->typeflag[0] == '2')
            {
                if (symlink(header_struct->linkname, full_name) == -1)
                {
                    fprintf(stderr, "Can not create Symlink.\n");
                    continue;
                }
            }
        }
        /* skip data otherwise*/
        else
        {
            /* extract number of data blocks to skip to read next header
             * (header_struct->size +BLOCK_SIZE -1) / BLOCK_SIZE round up to
             * next integer*/
            /* 8 is base used for octal conversion*/

            num_data_blocks =
                (strtol(header_struct->size, NULL, 8) + BLOCK_SIZE - 1) /
                BLOCK_SIZE;

            /* seek file to next header location*/

            lseek(tar_filedes, num_data_blocks * BLOCK_SIZE, SEEK_CUR);
        }

        /* free header_struct*/
        free(header_struct);
        /* check if end of archive is reached and end*/
        if (check_archive_end(tar_filedes))
        {
            break;
        }
    }

    /* seek back to start to seek more paths*/
    lseek(tar_filedes, 0, SEEK_SET);
}

void set_mod_time(char *full_name, time_t mod_time)
{
    struct stat stat_struct;
    /* first get access time to leave unchanged*/
    if (stat(full_name, &stat_struct) == -1)
    {
        fprintf(stderr, "Couldn't stat file for mod time.\n");
    }
    struct utimbuf time_buf;
    time_buf.actime = stat_struct.st_atime;
    time_buf.modtime = mod_time;

    if (utime(full_name, &time_buf) == -1)
    {
        fprintf(stderr, "Could not change mod time.\n");
    }
}

int make_nested_directory(char *full_name)
{
    if (mkdir(full_name, (S_IRWXU | S_IRWXG | S_IRWXO)) == 0)
    {
        return 0;
    }
    else
    {
        if (errno == EEXIST)
        {
            /*Directory already exists*/
            return 0;
        }
        else if (errno == ENOENT)
        {
            /* Parent directory doesn't exist, try creating it first*/
            char parent[PATH_LENGTH];
            strcpy(parent, full_name);

            /* Find the last occurrence of '/' in the path*/
            char *lastSlash = strrchr(parent, '/');
            if (lastSlash != NULL)
            {
                /* Null-terminate the parent directory path*/
                *lastSlash = '\0';

                // Recursively create parent directories
                if (make_nested_directory(parent) == 0)
                {
                    /* Parent directories created successfully, try creating
                     * the current directory again*/
                    if (mkdir(full_name, (S_IRWXU | S_IRWXG | S_IRWXO)) == 0)
                    {
                        return 0;
                    }
                    else
                    {
                        perror("mkdir");
                        return -1;
                    }
                }
                else
                {
                    return -1;
                }
            }
            else
            {
                /* No '/' found, likely an issue with the path*/
                fprintf(stderr, "Invalid path.\n");
                return -1;
            }
        }
        else
        {
            /* Other error occurred*/
            perror("mkdir");
            return -1;
        }
    }
}
