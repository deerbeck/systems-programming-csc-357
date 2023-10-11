#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*Maxline that can be handled/ size of the buffer string*/
#define MAX_LINE_LENGTH 1000


char *read_long_line(FILE *file)
{
  /*variable to store needed space for memory allocation*/
  int num_chars = 0;

  /*buffer array initialized to 0 to buffer read line into*/
  char *buffer = (char*) calloc(MAX_LINE_LENGTH, 1);
  
  /*buffer char for fgetc*/
  char c;

  /*pointer to string where line is gonna be stored in*/
  char *line = NULL;
  
  /*loop through line to count chars for memory allocation
    until you get to a new-line character or EOF and put 
    the character into the buffer.
    But do not put new_line character in buffer*/
  /* this could all simply be done with fgets aswell*/
  while(((c = fgetc(file)) != EOF ) &&
	(c != '\n'))
    {
      buffer[num_chars] = c;
      num_chars++;    
    }

  /* Handle EOF Case*/
  if(c == EOF)
    {
      return NULL;
    }

  /* allocate space which was determined in num_chars
     as well as account for newline and null character*/
  line = (char*) malloc(num_chars+2);

  /* copy buffer into heap memory*/
  strcpy(line, buffer);

  /* free buffer*/
  free(buffer);
  
  return line;
}
  
 

int main(int argc, char *argv[])
{
  FILE *input_file = NULL;
  FILE *output_file = NULL;

  /* no args, use standard input*/
  if(argc == 1)
    {
      input_file = stdin;
      output_file = stdout;
    }
  
  /* if 2 args there only is an input file*/
  else if(argc == 2)
    {
      /* open file and safe its pointer*/
      input_file = fopen(argv[1], "r");
      output_file = stdout;
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
  /*print out first current_line*/
  fprintf(output_file,"%s\n", current_line);

  /*print current line to output*/
  if(!current_line)
    {
      /* NULL was read on the first line --> no content*/
      fprintf(output_file, "No file content.");
    }
      
  else
    {
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
    }
  return 0;
      
}
