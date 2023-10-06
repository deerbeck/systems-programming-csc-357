/*Written by Johannes Hirschbeck (jhirsc01)
  This program replaces tabs in its input with the proper number 
  of whitespaces until the next tab stop.*/
#include <stdio.h>

/* use macro for variable Tab_Width*/
#define TAB_WIDTH 8

int main(int argc, char *argv[])
{
  /* initialite variables*/
  int c;      /* character that was 
		 read and will be written*/
  int col = 0;/* column cursor to 
		 monitor current position*/
  int i = 0;/* loop variable*/
  int buffer = 0;/* buffer variable where needed 
		    whitespaces are safed*/

  while ((c=getchar()) != EOF)
    {

      switch(c)
        {
	  /* Tab Case*/
        case '\t':
	  /* safe needed whitespaces for next tabstop to buffer*/
	  buffer = (TAB_WIDTH- (col % TAB_WIDTH));
	  /* put whitespace and increment column cursor*/
	  for(i = 0; i < buffer; i++)
            {
	      putchar(' ');
	      col++;
            }

	  break;

	  /* Backspace Case*/
        case '\b':
	  /* Putchar and decrement cursor because 
	     backspace moves curser back 1*/
	  putchar(c);
	  col--;
	  /* handle csae when cursor 
	     is already on left margin*/
	  if(col < 0)
            {
	      col = 0;
            }
	  break;

	  /* New-Line Case and Return Case can be handled
	     in 1 because we are only interested in cursor position*/
        case '\n':
	  /* do not break and put infront of '\r' case 
	     to pass through and safe some lines*/
        case '\r':
	  /* new line/ carriage return resets the 
	     col "cursor" to 0 and puts the \r out*/
	  col = 0;
	  putchar(c);
	  break;

	  /* Default-Case if none of the above assume normal 
	     character, put it and increment column cursor*/
        default:
	  putchar(c);
	  col++;
	  break;
        }
    }
  return 0;
}
