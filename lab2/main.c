/***Written by Johannes Hirschbeck (jhirsc01)

  This program is a version of the unix utility program uniq(1).
  This program will act as a filter, removing adjacent duplicate 
  lines as it copies its stdin to its stdout. That is, any line 
  that is identical to the previous line will be discarded rather
  than copied to stdout.

Arguments: <input> <output>
Return: 0 on success
 ***/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_long_line.h"

int main(int argc, char *argv[])
{
    /* no args, use standard input*/
    FILE *input_file = stdin;
    FILE *output_file = stdout;


    /* if 2 args there is only an input file*/
    if(argc == 2)
    {
        /* open file and safe its pointer*/
        input_file = fopen(argv[1], "r");
    }
    /* if 3 args there is an input and an output file*/
    else if(argc == 3)
    {
        /* open file and safe its pointer*/
        input_file = fopen(argv[1], "r");
        /* open output file and safe its pointer*/
        output_file = fopen(argv[2], "w");
    }
    /* handle error occurance in opening the files*/
    if(input_file == NULL || output_file == NULL)
    {
        perror("error handling file");
        exit(EXIT_FAILURE);
    }

    /* current line */
    char *current_line;

    /* next line */
    char *next_line;


    /*read first line*/
    current_line = read_long_line(input_file);
    if(current_line)
    {
        /*print out first current_line*/
        fprintf(output_file,"%s\n", current_line);
    }

    else
    {
        /*end program when no input content is given*/
        return 0;
    }

    /* read next line and compare it to current line
       do it in a for loop to loop through all next lines*/
    while(1)
    {
        next_line = read_long_line(input_file);
        /* EOF was reached*/
        if(!next_line)
        {
            /* free next_line memory before exiting*/
            free(next_line);
            break;
        }
        /* compare current_line and next_line and only check 
           if it is not the same*/
        if(strcmp(current_line, next_line) != 0)
        {
            /* realloc current_line memory to fit in new next_line*/
            current_line = realloc(current_line, strlen(next_line)+1);

            /* copy next_line and set as new current_line*/
            strcpy(current_line, next_line);

            /* free next_line because new memory will be allocated for next
               reading*/
            free(next_line);

            /*print out new current_line*/
            fprintf(output_file,"%s\n", current_line);
        }
    }

    /*freeing allocated memory after being done with comparison*/
    free(current_line);      
    return 0;
}
