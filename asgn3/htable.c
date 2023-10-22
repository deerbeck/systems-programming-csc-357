/***Written by Johannes Hirschbeck (jhirsc01). This program, htable generates
 * the table of encodings appropriate for a given file.
 * Usage: htable filename
 ***/

#include "utility.h"

int main(int argc, char *argv[])
{
    /* input file has to be given for encoding*/
    int input_fd;

    /* handle missing input file*/
    if (argc != 2)
    {
        printf("Usage: %s <input_file>", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* open input file and handle any occuring error while opening*/
    else
    {
        input_fd = open(argv[1], O_RDONLY);

        if (input_fd == -1)
        {
            perror(argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    /* get histogram of input*/
    int *hist = histogram(input_fd);

    /* create linked list out of previous generated histogram*/
    node *head = linkedList(hist);

    /* if file is empty the head is gonna be NULL -> */
    if (!head)
    {
        /* when no content is detected in the file open() just creates an empty
         * file as intended*/
        /* free all allocated memory until this point*/
        /* linked list does not have to be freed because no node was created*/
        /* free memory of histogram*/
        free(hist);
        /*close input and output file*/
        close(input_fd);
        return 0;
    }

    /* loop through linked list to get number of entries*/
    int num = 0;
    node *count_node = head;
    while (count_node)
    {
        count_node = count_node->next;
        num += 1;
    }
    
    /* create binaryTree out of linked list*/
    node *root = binaryTree(head);

    /* populate hash table with blank entries*/
    h_table_entry *h_table[num];
    int i;
    for (i = 0; i < num; i++)
    {
        h_table[i] = (h_table_entry *)malloc(sizeof(h_table_entry));
    }

    /* create h_table from binary tree*/
    /* index and path variables needed for initialization of the function*/
    int index = 0;
    char path[256] = "";
    populateHTable(root, h_table, path, &index);

    /* sort h_table in ascending order of the byte values*/
    hTableSort(h_table, num);

    int h_index;
    for (h_index = 0; h_index < num; h_index++)
    {
        printf("0x%02x: %s\n", h_table[h_index]->byte, 
        h_table[h_index]->encoding);
    }

    /* free memory*/
    /* free binary tree*/
    freeBinaryTree(root);
    /* free histogram*/
    free(hist);
    /*close input and output file*/
    close(input_fd);
    return 0;
}
