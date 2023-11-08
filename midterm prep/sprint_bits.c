#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

char *sprint_bits(char *buffer, unsigned int num)
{
    int i;
    for (i = 0; i < 32; i++)
    {
        if ((num >> (31 - i)) & 1)
            buffer[i] = '1';
        else
            buffer[i] = '0';
    }
    buffer[32] = '\0';
    return buffer;
}
int main(int argc, char *argv[])
{
    unsigned int num = 0xFFFFFFFF;
    char bitstr[33];
    sprint_bits(bitstr, num);
    printf("%u is %s.\n", num, bitstr);
}
