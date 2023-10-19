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
    /* input file has to be given for encoding*/
    int input_fd;
    /* if no output file is given, use stdout*/
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
            output_fd = open(argv[2], O_WRONLY | O_CREAT);

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
    node *first_el = linkedList(hist);

    /* print out items of linked list*/
    printList(first_el);

    /* if file is empty the head is gonna be NULL -> */
    if (!first_el)
    {
        /* TO DO*/
        /* return an empty file*/
        printf("Empty file\n");
        return 0;
    }

    /* loop through linked list to get number of entries*/
    int num_bytes = 0;
    node *count_node = first_el;
    while (count_node)
    {
        count_node = count_node->next;
        num_bytes += 1;
    }

    /* create binaryTree out of linked list*/
    node *root = binaryTree(first_el);

    /* print out binary Tree*/
    printBinaryTree(root);

    /* populate hash table with blank entries*/
    h_table_entry *h_table[num_bytes];
    int i;
    for (i = 0; i < num_bytes; i++)
    {
        h_table[i] = (h_table_entry *)malloc(sizeof(h_table_entry));
    }

    /* create h_table from binary tree*/
    /* index and path variables needed for initialization of the function*/
    int index = 0;
    char path[256] = "";
    populateHTable(root, h_table, path, &index);

    /* sort h_table in ascending order of the byte values*/
    hTableSort(h_table, num_bytes);

    /* TODO*/
    /* apply header to output*/

    /* apply encoding to output*/
    applyEncoding(input_fd, output_fd, h_table, num_bytes);

    /* free all nodes of sorted linked list*/
    /*free_list(first_el);*/

    /* free h_table*/
    for (i = 0; i < num_bytes; i++)
    {
        free(h_table[i]);
    }
    /* free memory of histogram*/
    free(hist);
    /*close input and output file*/
    close(input_fd);
    close(output_fd);

    return 0;
}