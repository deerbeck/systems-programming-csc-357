#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *read_long_line(FILE *file);
    /* This function reads a line from the input stream, the New-line
     * character and outputs the line which is saved on dynamically
     * allocated
     * memory on the heap.
     * Arguments: FILE Pointer to input stream.
     * Return: char Pointer to the read line.
     */
