#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>

/* define PATHMAX as 2048 if PATH_MAX is undefined*/
#ifndef PATH_MAX
#define PATH_MAX 2048
#endif

int main(int argc, char *argv[])
{
    /* initialize variables*/
    DIR *current_dirp;
    struct dirent *current_entry;
    struct stat child_stat, current_stat, root_stat;
    int path_length = 0;

    /* PATH_MAX +1 to include nullterminator*/
    /* use to not need to nullterminate string*/
    char *path_name = (char *)calloc(PATH_MAX + 1, sizeof(char));
    if (path_name == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* get the root stat to later check if root was reached*/
    if (lstat("/", &root_stat) == -1)
    {
        perror("Cannot get current working directory.");
        exit(EXIT_FAILURE);
    }

    /* traverse back in the directories*/
    while (1)
    {
        /* open currents directorys stats to get inode and device number
         * this will soon become the child*/
        if (lstat(".", &child_stat) == -1)
        {
            perror("Cannot get current working directory.");
            exit(EXIT_FAILURE);
        }

        /* check if we hit root break out of the while loop*/
        if ((root_stat.st_ino == child_stat.st_ino) &&
            (root_stat.st_dev == child_stat.st_dev))
        {
            break;
        }

        /* change current directory to parent directory to check for its
        entries*/
        if (chdir("..") == -1)
        {
            perror("Cannot get current working directory.");
            exit(EXIT_FAILURE);
        }

        /* open current directory to compare child entries*/
        /* handle errors accordingly*/
        current_dirp = opendir(".");
        if (current_dirp == NULL)
        {
            perror("mypwd");
            exit(EXIT_FAILURE);
        }

        /* loop through the directory entries and find name of current
         * directory by comparing the entries inode with current directories
         * inode as well as comparing the device numbers*/
        while ((current_entry = readdir(current_dirp)) != NULL)
        {
            /* stat the current entry to get i node and device number*/
            /* cannot rely on d_ino if we are on different devices*/
            if (lstat(current_entry->d_name, &current_stat) == -1)
            {
                perror("Cannot get current working directory.");
                exit(EXIT_FAILURE);
            }

            if (current_stat.st_ino == child_stat.st_ino &&
                current_stat.st_dev == child_stat.st_dev)
            {
                /* get new path length to check if path is too long and for use
                 * in the tmp allocation*/
                /* buffer is at least the size of pathname, the new name and 2
                 * more for the '/' character and NUL*/
                path_length = strlen(current_entry->d_name) +
                              strlen(path_name) + 2;

                /* check if path would get too long*/
                if (path_length >= PATH_MAX)
                {
                    fprintf(stderr, "path too long\n");
                    exit(EXIT_FAILURE);
                }

                /* a bit messy but it gets the job done*/
                /* get temporary string to store name in*/
                char tmp[path_length];
                /* clean data pls*/
                memset(tmp, '\0', path_length);
                /* start with the '/'*/
                strcpy(tmp, "/");
                /* append the new name*/
                strcat(tmp, current_entry->d_name);
                /* append the rest of the path_name*/
                strcat(tmp, path_name);
                /* cope tmp to path_name and free the buffer*/
                strcpy(path_name, tmp);
                /* break out of innerwhile loop because the right entry was
                found*/
                break;
            }
        }
        /* check if directory maybe unlinked (it wasn't found in the
         * direntries) --> current entry is empty*/
        if(current_entry == NULL)
        {
            fprintf(stderr, "Cannot get current working directory.");
            exit(EXIT_FAILURE);
        }

        /* close direcotry*/
        if (closedir(current_dirp) == -1)
        {
            perror("Cannot get current working directory.");
            exit(EXIT_FAILURE);
        }
    }

    /* print out final path_name*/
    printf("%s\n", path_name);
    /* get pwd back to origin*/
    if (chdir(path_name) == -1)
    {
        perror("Cannot get current working directory.");
        exit(EXIT_FAILURE);
    }

    /* free path_name*/
    free(path_name);
    return 0;
}