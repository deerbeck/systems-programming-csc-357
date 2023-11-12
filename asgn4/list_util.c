#include "list_util.h"

void list_tar(char **shopping_list, int tar_filedes, int num_paths)
{
    /* initialize variables*/
    Header *header_struct;
    ssize_t bytes_read;
    char header_data[BLOCK_SIZE];
    int num_data_blocks;

    char full_name[PATH_LENGTH];

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
            fprintf(stderr, "Header not valid. I give up...\n");
            /* free header_struct*/
            free(header_struct);
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
        /* check if end of archive is reached and end*/
        if (check_archive_end(tar_filedes))
        {
            break;
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
     * for \0 character*/
    char owner[9];
    char group[9];
    char result[18];

    /* pls don't judge, I like clean data*/
    memset(owner, '\0', 9);
    memset(group, '\0', 9);
    memset(result, '\0', 18);

    /* check if username is stored and if not put out uid*/
    if (strlen(header_struct->uname) > 0)
    {
        strncpy(owner, header_struct->uname, 8);
    }
    else
    {
        sprintf(owner, "%7d", (int)strtol(header_struct->uid, NULL, 8));
    }
    /* check if group name is stored and if not put out gid*/
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
