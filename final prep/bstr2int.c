#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

unsigned int bstr2int(char *s)
{
    if (!s)
        return 0;
    unsigned int result = 0;
    int overflow = 0;
    int i = 0;

    while (s[i] == '0')
        i++;

    while (s[i] != '\0')
    {
        if (overflow > (sizeof(int) * 8 -1))
            return -1;
        if (s[i] == '1')
            result = (result << 1) | 1;
        else
            result = result << 1;
        overflow++;
        i++;
    }
    return result;
}
int main(int argc, char *argv[])
{
    printf("%d\n", bstr2int("01010101110100100"));
}
