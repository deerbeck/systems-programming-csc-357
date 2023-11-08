#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int parity(int val)
{
    int i;
    int count = 0;
    printf("0x%x\n", val);
    for (i = 0; i < (sizeof(int) * 8); i++)
    {
        if ((val >> i) & 1)
            count++;
    }
    return count % 2;
}
int main(int argc, char *argv[])
{
    printf("%d\n",parity(-1234123412));
}
