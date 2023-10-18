#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utility.h"

int main(int argc, char *argv[])
{
    /* open input file and handle any occuring error while opening*/
    FILE *in_file = fopen(argv[1], "rb");
    if (in_file == NULL)
    {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    /* get histogram of input*/
    int *hist = histogram(in_file);

    /* create linked list out of previous generated histogram*/
    node *first_el = linked_list(hist);

    /* print out items of linked list*/
    printList(first_el);

    /* free memory of histogram*/
    free(hist);
    
    /* free all nodes of sorted linked list*/
    free_list(first_el);

    return 0;
}