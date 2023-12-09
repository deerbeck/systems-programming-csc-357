#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

char set_even_parity(char b)
{
    int i;
    int count = 0;
    printf("0x%x\n", b);
    for (i = 0; i < 7; i++)
    {
        if ((b >> i) & 1)
            count++;
    }
    if (count % 2)
        b = b | (1 << 7);
    else

        b = b & ~(1 << 7);
    return b;
}
int main(int argc, char *argv[])
{
    printf("%d\n", set_even_parity(208));
}
