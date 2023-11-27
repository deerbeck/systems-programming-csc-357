#include "list_util.h"

void list_tar(char **shopping_list, int tar_filedes, int num_paths)
{
    /* initialize variables*/
    Header *header_struct;
    ssize_t bytes_read;
    char header_data[BLOCK_SIZE];
    int num_data_blocks;
    char full_name[PATH_LENGTH];

    /* end of archive flag*/
    int end_of_archive = 0;

    /* loop index*/
    int i;

    while ((bytes_read = read(tar_filedes, header_data, BLOCK_SIZE)) != 0)
    {
        /* if reading goes wrong...*/
        if (bytes_read == -1)
        {
            perror("Reading file");
            exit(EXIT_FAILURE);
        }

        /* reset name*/
        memset(full_name, '\0', PATH_LENGTH);

        /* get header struct out of read data and check*/
        header_struct = extract_header(header_data);
        if (!header_struct)
        {
            fprintf(stderr, "Error extracting Header Data. Skipping...\n");
            continue;
        }

        if (!validate_header(header_struct))
        {
            /* we may have hit the end of the archive*/
            if (end_of_archive == 0)
            {
                end_of_archive = 1;
                continue;
            }
            /* now if checksum is only the space values we know we hit another
             * \0 block*/
            else if (end_of_archive &&
                     (calc_checksum(header_struct) == (CHKSUM_SIZE * ' ')))
            {
                /* now check the data if it is all 0*/
                break;
            }
            else
            {
                /* we actually hit a bad header if it is not all \0*/

                fprintf(stderr, "Header not valid. I give up...\n");
                /* free header_struct*/
                free(header_struct);
                exit(EXIT_FAILURE);
            }
        }

        /* extract number of data blocks to skip to read next header
         * (header_struct->size +BLOCK_SIZE -1) / BLOCK_SIZE round up to next
         * integer*/
        /* 8 is base used for octal conversion*/
        num_data_blocks =
            (strtol(header_struct->size, NULL, OCTALFLAG) + BLOCK_SIZE - 1) /
            BLOCK_SIZE;

        /* seek file to next header location*/
        if (lseek(tar_filedes, num_data_blocks * BLOCK_SIZE, SEEK_CUR) == -1)
        {
            perror("lseek");
            exit(EXIT_FAILURE);
        }
        /* get full name to check for pathname and for later printing*/
        build_name(header_struct, full_name);

        /* look if element is in shoppinglist and print if so*/
        for (i = 0; i < num_paths; i++)
        {

            if (strstr(full_name, shopping_list[i]))
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
        }

        /* free header_struct*/
        free(header_struct);
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
        fprintf(stderr, "Filetype not supported");
        break;
    }
    /* pass mode to helper function 8 is base for octal*/
    print_permissions((mode_t)strtol(header_struct->mode, NULL, OCTALFLAG));
    /* put whitespace*/
    putchar(' ');
    /* print out owner / if not available the uid*/
    print_owner_group(header_struct);
    /* put whitespace*/
    putchar(' ');
    /*print out size of file*/
    printf("%7d", (int)strtol(header_struct->size, NULL, OCTALFLAG));
    /* put whitespace*/
    putchar(' ');
    /* print out modification time*/
    print_time((time_t)strtol(header_struct->mtime, NULL, OCTALFLAG));
    /* put whitespace*/
    putchar(' ');

    /* print out name of file*/
    printf("%s\n", full_name);
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
    /* length is taken from sepcification + 1 for '\0' character I chose to
     * truncate my names but have both at a max length of max 8*/
    char owner[USRNAME_LENGTH + 1];
    char group[GRPNAME_LENGTH + 1];
    char result[DISPLAYNAME_LENGTH + 1];

    /* please don't judge, I like clean data*/
    memset(owner, '\0', USRNAME_LENGTH + 1);
    memset(group, '\0', GRPNAME_LENGTH + 1);
    memset(result, '\0', DISPLAYNAME_LENGTH + 1);

    /* check if username is stored and if not print out uid*/
    if (strlen(header_struct->uname) > 0)
    {
        strncpy(owner, header_struct->uname, USRNAME_LENGTH);
    }
    else
    {
        sprintf(owner, "%7d",
                (int)strtol(header_struct->uid, NULL, OCTALFLAG));
    }
    /* check if group name is stored and if not print out gid*/
    if (strlen(header_struct->gname) > 0)
    {
        strncpy(group, header_struct->gname, GRPNAME_LENGTH);
    }
    else
    {
        sprintf(group, "%7d",
                (int)strtol(header_struct->gid, NULL, OCTALFLAG));
    }

    /* copy to result and print out in the end*/
    strcpy(result, owner);
    strcat(result, "/");
    strcat(result, group);
    /* 17 is from the specification*/
    printf("%-17s", result);
}

void print_time(time_t time)
{
    /* Convert the time to a struct tm in local time*/
    struct tm *localTime = localtime(&time);

    /* Check if localtime failed*/
    if (localTime == NULL)
    {
        perror("Error getting local time");
    }

    /* Format the time as a string */
    char formattedTime[20]; /* YYYY-MM-DD HH:MM\0*/
    strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M",
             localTime);

    /* Print the formatted time*/
    printf("%s", formattedTime);
}
