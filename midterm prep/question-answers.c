#include <stdio.h>
#include <string.h>

int three(int x)
{
    return x + x + x;
}
#define one(x) (x * x)
#define two(y, x) ((x) ? (x) : (y))

int main(int argc, char *argv[])
{

    int x = 3;
    int y = 4;
    /*printf("%d\n", A - B);*/
    printf("%d\n", one(2 + 3));
    printf("%d\n", two(x, y));
    printf("%d\n", three(++x));
    printf("%d\n", one(two(0, 1)));

    return 0;
}

size_t strspn(const char *s, const char *accept)
{
    size_t seg_len = 0;
    if (!s || !accept)
    {
        return -1;
    }
    int s_len = strlen(s);
    int a_len = strlen(accept);
    int i, j;
    int start_flag = 0;
    for (i = 0; i < s_len; i++)
    {
        for (j = 0; j < a_len; j++)
        {
            if (start_flag == 0 && s[i] == accept[j])
            {
                start_flag = 1;
                seg_len++;
            }
            else if (start_flag == 1 && s[i] == accept[j])
            {
                seg_len++;
            }
            else if (start_flag == 1 && s[i] != accept[j])
            {
                return seg_len;
            }
        }
    }
    return seg_len;
}