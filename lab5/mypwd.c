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
    char *path_name = (char *)calloc(PATH_MAX + 1, 1);
    if (path_name == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* get the root stat to check if root was reached*/
    if (stat("/", &root_stat) == -1)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    /* traverse back in the directories*/
    while (1)
    {
        /* open currents directorys stats to get inode and device number
         * this will soon become the child*/
        if (stat(".", &child_stat) == -1)
        {
            perror("cannot get current working directory.");
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
            perror("cannot get current working directory.");
            exit(EXIT_FAILURE);
        }

        /* open current directory to compare child entries*/
        /* open currents directory parent directory*/
        /* handle errors accordingly*/
        current_dirp = opendir(".");
        if (current_dirp == NULL)
        {
            perror("mypwd");
            exit(EXIT_FAILURE);
        }

        /* loop through the directory entries and find name of current
         * directory by comparing the entries inode with current directories
         * inode as well as comparing the ineds*/
        while ((current_entry = readdir(current_dirp)) != NULL)
        {
            /* stat the current entry to get i node and device number*/
            if (stat(current_entry->d_name, &current_stat) == -1)
            {
                perror("cannot get current working directory.");
                exit(EXIT_FAILURE);
            }

            if (current_stat.st_ino == child_stat.st_ino &&
                current_stat.st_dev == child_stat.st_dev)
            {
                /* get new path length to check if path is too long and for use
                 * in the tmp allocation*/
                /* buffer is at least the size of pathname, the new name and 1
                 * more for the '/' character*/
                path_length = strlen(current_entry->d_name) +
                              strlen(path_name) + 1;

                /* check if path would get too long*/
                if (path_length >= PATH_MAX)
                {
                    fprintf(stderr, "path too long\n");
                    exit(EXIT_FAILURE);
                }

                /* a bit messy but it gets the job done*/
                /* get temporary string to store name in*/
                char tmp[path_length];
                /* start with the '/'*/
                strcpy(tmp, "/");
                /* append the new name*/
                strcat(tmp, current_entry->d_name);
                /* append the rest of the path_name*/
                strcat(tmp, path_name);
                /* cope tmp to path_name and free the buffer*/
                strcpy(path_name, tmp);
                /* break out of while loop because obviously the right entry
                was found*/
                break;
            }
        }

        /* close direcotry*/
        if (closedir(current_dirp) == -1)
        {
            perror("cannot get current working directory.");
            exit(EXIT_FAILURE);
        }
    }

    /* print out final path_name*/
    printf("%s\n", path_name);
    /* get cwd back to origin*/
    if (chdir(path_name) == -1)
    {
        perror("cannot get current working directory.");
        exit(EXIT_FAILURE);
    }

    /* free path_name*/
    free(path_name);
    return 0;
}

/* start at where you're at --> go 1 derectory up ("..") --> look for which
 direntry has the same i node as where you came from --> get the d_name from
 that entry --> put it in path_max (at the end maybe or concatenate or
 something) --> change direcotry to where you are right now with chdir --> do
 the same thing again until you hit root (inode is the same)*/
