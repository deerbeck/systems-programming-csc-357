#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

unsigned char make_byte(const int bits[])
{
    unsigned char result = 0;
    int i;
    for(i = 0; i<8;i++)
    {
        result = result | (bits[i] << (7-i));
    }
    return result;
}

int main(int argc, char *argv[])
{
    int bits [] = {1,1,0,0,0,1,0,1};
    printf("0x%x\n", make_byte(bits));
}