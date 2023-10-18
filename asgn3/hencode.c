#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "utility.h"

int main(int argc, char *argv[])
{
    int input_fd;
    int output_fd = 1;

    /* handle missing input file*/
    if (argc == 1)
    {
        printf("Usage: %s <input_file> [<output_file>]", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* open input file and handle any occuring error while opening*/
    else if (argc > 1)
    {
        input_fd = open(argv[1], O_RDONLY);

        if (input_fd == -1)
        {
            perror(argv[1]);
            exit(EXIT_FAILURE);
        }
        /* if output file provided open it and handle occuring error while
         * opening*/
        if (argc == 3)
        {
            output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

            if (output_fd == -1)
            {
                perror(argv[2]);
                /* close input file before exiting*/
                close(input_fd);
                exit(EXIT_FAILURE);
            }
        }
    }

    /* get histogram of input*/
    int *hist = histogram(input_fd);

    /* create linked list out of previous generated histogram*/
    node *first_el = linked_list(hist);

    /* if file is empty the head is gonna be NULL -> */
    if(!first_el)
    {   
        /* TO DO*/
        /* return an empty file*/
        printf("Empty file\n");
        return 0;
    }

    /* print out items of linked list*/
    printList(first_el);

    /* free memory of histogram*/
    free(hist);

    /* free all nodes of sorted linked list*/
    free_list(first_el);

    /*close input and output file*/
    close(input_fd);
    close(output_fd);

    return 0;
}