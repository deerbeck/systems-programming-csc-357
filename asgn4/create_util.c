#include "create_util.h"

void archive_file(const char *pathname, int tar_filedes)
{
    /* check if pathname is too long*/
    if (strlen(pathname) > PATH_LENGTH - 1)
    {
        fprintf(stderr, "Pathname too long. Skipping...\n");
        return;
    }
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
            fprintf(stderr, "%s could not be opened. Skipping...",
                    path_buffer);
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
        /* close dir after we're done with it*/
        if (closedir(current_dir) == -1)
        {
            perror("closedir");
            exit(EXIT_FAILURE);
        }
    }
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
    if (strlen(path_buffer) > NAME_SIZE)
    {
        char *split_pos = path_buffer;
        char *end_path = strchr(path_buffer, '\0');

        split_pos = strchr(split_pos, '/');
        while (end_path - split_pos > NAME_SIZE)
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
        strncpy(header_struct->name, split_pos + 1, NAME_SIZE);

        /* put rest of name in prefix*/
        strncpy(header_struct->prefix, path_buffer, split_pos - path_buffer);
    }
    else
    {
        strncpy(header_struct->name, path_buffer, NAME_SIZE);
    }
    /* add '/' if directory*/
    if (S_ISDIR(stat_struct.st_mode) &&
        strlen(header_struct->name) < NAME_SIZE)
    {
        strcat(header_struct->name, "/");
    }

    /* saving the mode print into mode array and strip of filetype*/
    /* 07777 is mask to only safe last 4 octal digits of mode_t*/
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
        if (readlink(path_buffer, header_struct->linkname,
                     LINKNAME_SIZE) == -1)
        {
            fprintf(stderr, "Can not read link. Skipping...");
        }
    }

    /* store magic "ustar"*/
    strncpy(header_struct->magic, "ustar", MAGIC_SIZE);

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
        strncpy(header_struct->uname, pwd->pw_name, UNAME_SIZE);
    }

    /* get group name and store in header*/
    struct group *grp = getgrgid(stat_struct.st_gid);
    if (!grp)
    {
        fprintf(stderr, "Groupname couldn't be fetched.\n");
    }
    else
    {
        strncpy(header_struct->gname, grp->gr_name, GNAME_SIZE);
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