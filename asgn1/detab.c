/*Written by Johannes Hirschbeck (jhirsc01)
This program replaces tabs in its input with the proper number of blanks
to space to the next tab stop.*/

#include <stdio.h>

#define TAB_WIDTH 8

int main(int argc, char *argv[])
{
    int c;
    int col = 0;
    int i = 0;
    int buffer = 0;

    while ((c=getchar()) != EOF)
    {

        switch(c)
        {
        case '\t':
            buffer = (TAB_WIDTH-(col % TAB_WIDTH));
            for(i = 0; i < buffer; i++)
            {
                putchar(' ');
                col++;
            }

            break;

        case '\b':
            putchar(c);
            col--;
            if(col < 0)
            {
                col = 0;
            }
            break;
        case '\n':
            col = 0;
            putchar(c);
            break;

        case '\r':
            col = 0;
            putchar(c);
            break;

        default:
            putchar(c);
            col++;
            break;
        }
    }
    return 0;
}
