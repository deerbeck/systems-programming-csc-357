/***Written by Johannes Hirschbeck (jhirsc01) This program is a version of the
 * unix utility program uniq(1).
 * This program will act as a filter, removing adjacent duplicate 
 * lines as it copies its stdin to its stdout. That is, any line 
 * that is identical to the previous line will be discarded rather
 * than copied to stdout.
 * Arguments: <input> <output>
 * Return: 0 on success
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


    /*read first line*/
    current_line = read_long_line(stdin);
    if(current_line)
    {
        /*print out first current_line*/
        printf("%s\n", current_line);
    }

    /* read next line and compare it to current line do it in a loop to loop
     * through all next lines break out if next line is NULL*/
    while((next_line = read_long_line(stdin)))
    {

        /* compare current_line and next_line and only check 
           if it is not the same*/
        if(strcmp(current_line, next_line) != 0)
        {
            /* realloc() current_line memory to fit in new next_line*/
            current_line = realloc(current_line, strlen(next_line)+1);

            /* copy next_line and set as new current_line*/
            strcpy(current_line, next_line);

            /* free next_line because new memory will be allocated for next
             * reading*/
            free(next_line);

            /*print out new current_line*/
            printf("%s\n", current_line);
        }
    }

    /*freeing allocated memory after being done with comparison*/
    free(next_line);
    free(current_line);      
    return 0;
}
