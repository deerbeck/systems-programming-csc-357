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
    DIR *parent_dirp;
    struct dirent *parent_entry;
    struct stat current_stat, parent_stat;
    int path_length = 0;
    /* PATH_MAX +1 to include nullterminator*/
    /* use to not need to nullterminate string*/
    char *path_name = (char *)calloc(PATH_MAX + 1, 1);
    if (path_name == NULL)
    {
        perror("memory allocation error");
        exit(EXIT_FAILURE);
    }

    /* traverse back in the directories*/
    while (1)
    {
        /* open currents directorys stats to get inode and device number*/
        if (lstat(".", &current_stat) == -1)
        {
            perror("lstat");
            exit(EXIT_FAILURE);
        }
        /* get parent stat aswell to check if we hit the root directory as well
        as comparing the device numbers*/
        if (lstat("..", &parent_stat) == -1)
        {
            perror("lstat");
            exit(EXIT_FAILURE);
        }

        /* check if we hit root and break out of the while loop*/
        if (parent_stat.st_ino == current_stat.st_ino)
        {
            break;
        }
        /* open currents directory parent directory*/
        /* handle errors accordingly*/
        parent_dirp = opendir("..");
        if (parent_dirp == NULL)
        {
            perror("mypwd");
            exit(EXIT_FAILURE);
        }

        /* loop through the directory entries and find name of current
         * directory by comparing the entries inode with current directories
         * inode*/
        while ((parent_entry = readdir(parent_dirp)) != NULL)
        {
            if (parent_entry->d_ino == current_stat.st_ino &&
                parent_stat.st_dev == current_stat.st_dev)
            {
                /* get new path length to check if path is too long and for use
                 * in the tmp allocation*/
                /* buffer is at least the size of pathname, the new name and 1
                 * more for the '/' character*/
                path_length = strlen(parent_entry->d_name) +
                              strlen(path_name) + 1;

                /* check if path would get too long*/
                if (path_length >= PATH_MAX)
                {
                    fprintf(stderr, "path too long\n");
                    exit(EXIT_FAILURE);
                }
                /* get temporary string to store name in*/
                char *tmp = (char *)malloc(path_length);
                /* handle memory allocation error*/
                if (tmp == NULL)
                {
                    perror("memory allocation error");
                    exit(EXIT_FAILURE);
                }

                /* start with the '/'*/
                strcpy(tmp, "/");
                /* append the new name*/
                strcat(tmp, parent_entry->d_name);
                /* append the rest of the path_name*/
                strcat(tmp, path_name);
                /* cope tmp to path_name and free the buffer*/
                strcpy(path_name, tmp);
                free(tmp);
                /* break out of while loop*/
                break;
            }
        }

        /* close direcotry*/
        if (closedir(parent_dirp) == -1)
        {
            perror("closedir");
            exit(EXIT_FAILURE);
        }

        /* change working directory*/
        chdir("..");
    }

    /* print out final path_name*/
    printf("%s\n", path_name);
    /* free path_name*/
    free(path_name);
    return 0;
}

/* start at where you're at --> go 1 derectory up ("..") --> look for which
 direntry has the same i node as where you came from --> get the d_name from
 that entry --> put it in path_max (at the end maybe or concatenate or
 something) --> change direcotry to where you are right now with chdir --> do
 the same thing again until you hit root (inode is the same)*/