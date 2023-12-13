#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{

    int A = 2;
    char *cp = "abcdefg";
    int B = -1;
    int grade[5] = {4, 3, 2, 1, 0};

    // Answer: 0
    printf("%d\n", (3 / 6));

    // Answer: 1
    printf("%d\n", (A + B));

    A = 2;
    B = -1;

    // Answer: 2
    printf("%d\n", (+A++ + ++B));

    // Answer: 2
    printf("%d\n", (grade[2]));

    // Answer: 'e'
    printf("%c\n", (*(cp + 4)));

    // Answer: 0x2
    printf("%d\n", (int)(&grade[4] - &grade[2]));

    // Answer: 8 (sizeof(int) = 4 -> 8 bytes for distance of 2 addresses)
    printf("%d\n", ((int)&grade[4]) - ((int)&grade[2]));

    A = 2;
    B = -1;
    // Answer: -1
    printf("%d\n", ((A != A) ? A : B));

    // Answer: 7
    printf("%i\n", (strlen(cp)));

    // Answer: 5*4 = 20
    printf("%d\n", (sizeof(grade)));

    // Answer: looks at address of first element of grade (int 4) and only looks at 1 byte spaces
    // if stored in little endian (0x04000000) strlen would be 1, if stored in big endian: (0x00000004) it //would be 0 because of when strlen encounters a \0 character
    printf("%d\n", (strlen((char *)(&grade[0]))));

    return 0;
}