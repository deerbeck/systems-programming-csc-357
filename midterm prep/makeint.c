#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
typedef struct bit *bitlist;
struct bit
{
    int bit;
    bitlist next;
};
int makeint(bitlist bits)
{
    int result = 0;
    int count = 0;
    while (bits && bits->bit == 0)
        bits = bits->next;
    while (bits)
    {
        if (count >= 32)
            return -1;
        result = (result << 1) | (bits->bit);
        bits = bits->next;
        count++;
    }
    return result;
}
int main(int argc, char *argv[])
{
    struct bit b1;
    struct bit b2;
    struct bit b3;
    struct bit b4;
    struct bit b5;
    struct bit b6;
    struct bit b7;
    b1.next = &b2;
    b2.next = &b3;
    b3.next = &b4;
    b4.next = &b5;
    b5.next = &b6;
    b6.next = &b7;
    b7.next = &b6;
    b1.bit = 0;
    b2.bit = 0;
    b3.bit = 0;
    b4.bit = 1;
    b5.bit = 0;
    b6.bit = 1;
    b7.bit = 1;
    printf("%d\n", makeint(&b1));
}
