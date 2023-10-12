#include <stdio.h>

int main(int argc, char *argv[])
{

  int i, j = 0;
  for (i = 0; i < 100; j++) {
    j+=2;
    printf("Hello World!\n");
  }
  printf("The value of j is: %d\n",j);
  return 0;
}
