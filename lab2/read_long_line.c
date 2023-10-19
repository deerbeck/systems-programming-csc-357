#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "read_long_line.h"

char *read_long_line(FILE *file)
{
    /* variable to store needed space for memory allocation*/
    int num_chars = 0;

    /* initial line size*/
    int line_length = 1000;

    /* buffer array initialized to \0 to buffer read line into*/
    char *line = (char *)calloc(line_length, 1);

    /* temp pointer for reallocation error handling*/
    char *tmp_ptr;

    /* buffer char for fgetc()*/
    char c;

    /* handle memory allocation error */
    if (line == NULL)
    {
        exit(EXIT_FAILURE);
    }

    /* loop through line to count chars for memory allocation
     * until you get to a new-line character or EOF and put
     * the character into the buffer.
     * But do not put new_line character in buffer*/
    while (((c = fgetc(file)) != EOF) && (c != '\n'))
    {
        /* write current character to line array and increment character
         * count*/
        line[num_chars] = c;
        num_chars++;

        if (num_chars >= line_length)
        {
            /* resize line_length as soon as char count exceeds original
             * line_length and then reallocate the line memory heap
             * accordingly */
            line_length += 500;
            tmp_ptr = (char *)realloc(line, line_length);

            /*handle memory allocation error */
            if (tmp_ptr == NULL)
            {
                free(line);
                exit(EXIT_FAILURE);
            }
            /* if reallocation successfull*/
            line = tmp_ptr;

            /* set remaining memory to \0 character*/
            memset((line + num_chars), '\0', (line_length - num_chars - 1));
        }
    }

    /* Handle EOF Case*/
    if (num_chars == 0 && c == EOF)
    {
        free(line);
        return NULL;
    }

    return line;
}
