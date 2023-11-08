#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    char *ﬁlename = "tf";
    int one, two, three;
    one = open(ﬁlename, O_RDWR);
    write(one, "cpe/csc", 7);
    write(one, "X317", 4);
    two = open(ﬁlename, O_WRONLY);
    three = dup(two);
    write(three, "new", 3);
    write(two, "Course", 6);
    lseek(one, 13, SEEK_SET);
    lseek(three, -6, SEEK_END);
    write(three, "number after", 6);
    lseek(two, 6, SEEK_SET);
    write(three, "summer", 3);
    write(one, "357?", 3);
    close(one);
    close(two);
    close(three);
    return 0;
}