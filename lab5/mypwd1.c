#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#define INITIAL_BUFFER_SIZE 1024

char* getcwd_custom() {
    char* cwd = NULL;
    struct stat current_stat;
    struct dirent* entry;
    DIR* dir = NULL;
    size_t cwd_size = INITIAL_BUFFER_SIZE;

    while (1) {
        if (cwd) {
            free(cwd);
        }

        cwd = (char*)malloc(cwd_size);
        if (!cwd) {
            perror("malloc");
            exit(1);
        }

        if (stat(".", &current_stat) == -1) {
            perror("stat");
            exit(1);
        }

        /* Check if the inode numbers of the current directory and the root directory are the same.*/
        if (current_stat.st_ino == current_stat.st_dev) {
            cwd[0] = '/';
            cwd[1] = '\0';
            break;
        }

        /* Open the parent directory.*/
        dir = opendir("..");
        if (!dir) {
            perror("opendir");
            exit(1);
        }

        /* Find the current directory entry within the parent directory.*/
        while ((entry = readdir(dir)) != NULL) {
            struct stat entry_stat;
            if (stat(entry->d_name, &entry_stat) == -1) {
                perror(entry->d_name);
                exit(1);
            }

            if (entry_stat.st_ino == current_stat.st_ino) {
                strcpy(cwd, entry->d_name);
                break;
            }
        }

        /* Close the parent directory.*/
        if (closedir(dir) == -1) {
            perror("closedir");
            exit(1);
        }

        /* Check if the current directory was found.*/
        if (cwd[0] == '\0') {
            fprintf(stderr, "Couldn't determine current directory.\n");
            exit(1);
        }
    }

    return cwd;
}

int main() {
    char* cwd = getcwd_custom();
    printf("Current working directory: %s\n", cwd);
    free(cwd);

    return 0;
}
