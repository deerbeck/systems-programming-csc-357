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

  while ((c=getchar()) != EOF)
  {

    switch(c)
      {
      case '\t':
	for(i = 0; i < (TAB_WIDTH-col);i++)
	  {
	    putchar(' ');
	  }
	col = 0;
	break;

        case '\b':
	  putchar(c);
	  col--;
	break;

        case '\n':
	  putchar(c);
	  col = 0;       
	break;

	case '\r':
	  putchar(c);
	  col = 0;     
	break;
	
      default:
	putchar(c);
	col++;
	if(col == 8) col = 0;
	break;
      }
  }
  return 0;
}
