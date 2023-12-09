#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int one, two, three;
    char *filename = "tf";
    one = open(filename, O_RDWR);
    write(one, "What again?", 3);
    two = open(filename, O_WRONLY | O_TRUNC);
    three = dup(one);
    write(three, "Didn't", 3);
    write(two, "We", 3);
    lseek(one, 13, SEEK_SET);
    lseek(two, 7, SEEK_SET);
    write(two, "just do", 5);
    lseek(three, -1, SEEK_CUR);
    write(three, "this?", 2);
    lseek(two, 5, SEEK_CUR);
    write(two, "hayfever?", 2);
    write(one, "atchoo?", 2);
    close(one);
    close(two);
    close(three);

    return 0;
}