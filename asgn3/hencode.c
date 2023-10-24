/***Written by Johannes Hirschbeck (jhirsc01). This program is a compression
 * tool, hencode, that will use Huffman codes to compress a given file.
 * Usage:
 * hencode infile [ outfile ]
 ***/
#include "utility.h"

int main(int argc, char *argv[])
{
    /* input file has to be given for encoding*/
    int input_fd;
    /* if no output file is given, use stdout*/
    int output_fd = STDOUT_FILENO;

    /* handle missing input file*/
    if (argc == 1)
    {
        fprintf(stderr,"Usage: %s <input_file> [<output_file>]", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* open input file and handle any occuring error while opening*/
    else if (argc > 1)
    {
        input_fd = open(argv[1], O_RDONLY);

        if (input_fd == -1)
        {
            perror("close");
            exit(EXIT_FAILURE);
        }
        /* if output file provided open it and handle occuring error while
         * opening*/
        if (argc == 3)
        {
            output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC,
                             S_IRUSR | S_IWUSR);

            if (output_fd == -1)
            {
                perror(argv[2]);
                /* close input file before exiting*/
                if (close(input_fd) == -1)
                {
                    perror("close");
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_FAILURE);
            }
        }
    }

    /* get histogram of input*/
    int *hist = histogram(input_fd);

    /* create linked list out of previous generated histogram*/
    node *head = linkedList(hist);

    /* if file is empty the head is gonna be NULL -> */
    if (head == NULL)
    {
        /* when no content is detected in the file open() just creates an empty
         * file as intended*/
        /* free all allocated memory until this point*/
        /* linked list does not have to be freed because no node was created*/
        /* free memory of histogram*/
        free(hist);
        /*close input and output file*/
        /*close input and output file*/
        if (close(input_fd) == -1)
        {
            perror("close");
            exit(EXIT_FAILURE);
        }
        if (close(output_fd) == -1)
        {
            perror("close");
            exit(EXIT_FAILURE);
        }
        return 0;
    }

    /* loop through linked list to get number of entries*/
    uint16_t num = 0;
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
    char path[NUM_POSSIB_BYTES] = "";
    populateHTable(root, h_table, path, &index);

    /* sort h_table in ascending order of the byte values*/
    qsort(h_table, num, sizeof(h_table_entry *), compareEnntries);

    /* apply header to output*/
    writeHeader(output_fd, hist, num);

    /* create bitsream for encoding*/
    bitstream *bs = createBitstream();

    /* creat h_table lookup for easier encoding*/
    /* h_table entry is store at the index equal to the byte*/
    h_table_entry *h_lookup[NUM_POSSIB_BYTES];
    for (i = 0; i < num; i++)
    {
        h_lookup[h_table[i]->byte] = h_table[i];
    }

    /* apply encoding to output*/
    generateEncoding(input_fd, output_fd, h_lookup, num, bs);

    /* write bitstream to output file*/
    writeEncoding(output_fd, bs);

    /* free h_table*/
    for (i = 0; i < num; i++)
    {
        free(h_table[i]);
    }

    /* free Bitstream and its data*/
    free(bs->data);
    free(bs);
    /* free binary tree*/
    freeBinaryTree(root);
    /* free memory of histogram*/
    free(hist);
    /*close input and output file*/
    if (close(input_fd) == -1)
    {
        perror("close");
        exit(EXIT_FAILURE);
    }
    if (close(output_fd) == -1)
    {
        perror("close");
        exit(EXIT_FAILURE);
    }
    return 0;
}
