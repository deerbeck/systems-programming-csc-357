/***Written by Johannes Hirschbeck (jhirsc01) This program is a version of the
 * unix utility program uniq(1). This program will act as a filter, removing
 * adjacent duplicate lines as it copies its stdin to its stdout. That is, any
 * line that is identical to the previous line will be discarded rather than
 * copied to stdout. Arguments: <input> <output> Return: 0 on success
 ***/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_long_line.h"

int main(int argc, char *argv[])
{
    /* init variablse*/
    /* current line */
    char *current_line;

    /* next line */
    char *next_line;

    /* read first line*/
    current_line = read_long_line(stdin);
    if (current_line)
    {
        /* print out first current_line*/
        printf("%s\n", current_line);
    }

    /* read next line and compare it to current line do it in a loop to loop
     * through all next lines break out if next line is NULL*/
    while ((next_line = read_long_line(stdin)))
    {

        /* compare current_line and next_line and only check if it is not the
           same*/
        if (strcmp(current_line, next_line) != 0)
        {

            /* free current line (memory managing) and swap it with next line*/
            free(current_line);
            current_line = next_line;

            /* print out new current_line*/
            printf("%s\n", current_line);
        }
        /* free memory if lines are the same*/
        else
        {
            free(next_line);
        }
    }

    /* freeing allocated memory after being done with comparison*/
    free(next_line);
    free(current_line);
    return 0;
}
