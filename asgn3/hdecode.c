/***Written by Johannes Hirschbeck (jhirsc01). This program decompression tool,
 * hdecode, that will take a input file which was compressed with huffman codes
 * and decompresses it.
 * Usage: hdecode [ ( infile | - ) [ outfile ] ]
 ***/
#include "utility.h"

int main(int argc, char *argv[])
{
    /* input file has to be given for encoding*/
    int input_fd = STDIN_FILENO;
    /* if no output file is given, use stdout*/
    int output_fd = STDOUT_FILENO;

    /* open input file if provided and handle any occuring error while opening*/
    if (argc > 1)
    {
        if (*argv[1] != '-')
        {
            input_fd = open(argv[1], O_RDONLY);

            if (input_fd == -1)
            {
                perror(argv[1]);
                exit(EXIT_FAILURE);
            }
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
                close(input_fd);
                exit(EXIT_FAILURE);
            }
        }
    }

    /* read Header and generate linked list*/
    node *head = readHeader(input_fd);

    /* create binary tree out of linked list*/
    node *root = binaryTree(head);

    /* create Bitstream where file is going to be stored in*/
    bitstream *bs = createBitstream();

    /* */
    decodeBody(input_fd, output_fd, root, bs);

    /*free memory*/
    /* free Bitstream and its data*/
    free(bs->data);
    free(bs);
    /* free binary tree*/
    freeBinaryTree(root);

    /*close input and output file*/
    close(input_fd);
    close(output_fd);

    return 0;
}