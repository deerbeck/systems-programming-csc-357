#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *read_long_line(FILE *file)
{
    /* This function reads a line from the input stream, the New-line
     * character and outputs the line which is saved on dynamically allocated
     * memory on the heap.
     * Arguments: FILE Pointer to input stream.
     * Return: char Pointer to the read line.
     */

    /*variable to store needed space for memory allocation*/
    int num_chars = 0;

    /*initial line size*/
    int line_length = 1024;

    /*buffer array initialized to \0 to buffer read line into*/
    char *line = (char*) calloc(line_length, 1);

    /*handle memory allocation error */
    if(line == NULL)
    {
        perror("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    /*buffer char for fgetc()*/
    char c;

    /* loop through line to count chars for memory allocation
     * until you get to a new-line character or EOF and put 
     * the character into the buffer.
     * But do not put new_line character in buffer*/
    /* this could all simply be done with fgets as well*/
    while(((c = fgetc(file)) != EOF ) && (c != '\n'))
    {
        line[num_chars] = c;
        num_chars++;   

        if(num_chars >= line_length)
        {
            /*resize line_length as char count exceeds original
             * line_length*/
            line_length += 100;
            line = (char*) realloc(line, line_length);

            /*handle memory allocation error */
            if(line == NULL)
            {
                perror("Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }


        }
    }

    /* Handle EOF Case*/
    if(c == EOF)
    {
        return NULL;
    }

    return line;
}
