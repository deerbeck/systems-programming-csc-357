#include "extract_util.h"

void extract_archive(char **shopping_list, int tar_filedes, int num_paths)
{
    /* initialize variables*/
    Header *header_struct;
    ssize_t bytes_read;
    ssize_t bytes_written;

    char header_data[BLOCK_SIZE];
    char file_data[BLOCK_SIZE];
    int num_data_blocks;
    int last_block_data_size;

    char full_name[PATH_LENGTH];

    int path_found_flag;
    int i, j;

    while ((bytes_read = read(tar_filedes, header_data, BLOCK_SIZE)) != 0)
    {
        /* reset flag*/
        path_found_flag = 0;
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
        for (j = 0; j < num_paths; j++)
        {
            if (strstr(full_name, shopping_list[j]))
            {
                path_found_flag = 1;
            }
        }
        /* extract if pathname given*/
        if (path_found_flag)
        {
            if (verbose)
            {
                printf("%s\n", full_name);
            }

            /* first check if directory and create it*/
            if (header_struct->typeflag[0] == '5')
            {
                stripLastCharacter(full_name);
                if (make_nested_directory(full_name) == -1)
                {
                    fprintf(stderr, "Couldn't create directory\n");
                    /* don't need to skip data because directory data size
                     * = 0*/
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
                /* check if file even is in subdirs and create them
                accordingly*/
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

                /* get correct permissions for file rw for everyone and
                 * execute for everyone if anyone has execute*/
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
                 * (header_struct->size +BLOCK_SIZE -1) / BLOCK_SIZE round
                 * up to next integer*/
                /* 8 is base used for octal conversion*/

                num_data_blocks =
                    (strtol(header_struct->size, NULL, 8) +
                     BLOCK_SIZE - 1) /
                    BLOCK_SIZE;

                /* get how much to write if the last block is not completly
                 * filled*/
                last_block_data_size = strtol(header_struct->size,
                                              NULL, 8) %
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
                             (time_t)strtol(header_struct->mtime,
                                            NULL, 8));
            }
            /* now handle symlinks*/
            else if (header_struct->typeflag[0] == '2')
            {
                if (symlink(header_struct->linkname, full_name) == -1)
                {
                    perror("symlink");
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
}

void stripLastCharacter(char *str)
{
    /* strip last character of string*/
    size_t len = strlen(str);
    if (len > 0)
    {
        str[len - 1] = '\0';
    }
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

                /* Recursively create parent directories*/
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