#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
struct node_st
{
    int data;
    struct node_st *next;
};

void print_reverse_list(struct node_st *l)
{
    if (!l)
        return;
    struct node_st *prev, *current, *next;
    prev = NULL;
    current = l;

    while (current)
    {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }

    while (prev)
    {
        printf("%d\n", prev->data);
        prev = prev->next;
    }
}

int main(int argc, char *argv[])
{
    struct node_st *t1 = (struct node_st *)malloc(sizeof(struct node_st));
    struct node_st *t2 = (struct node_st *)malloc(sizeof(struct node_st));
    struct node_st *t3 = (struct node_st *)malloc(sizeof(struct node_st));
    struct node_st *t4 = (struct node_st *)malloc(sizeof(struct node_st));
    char *s1 = (char *)malloc(10);
    char *s2 = (char *)malloc(10);
    char *s3 = (char *)malloc(10);
    char *s4 = (char *)malloc(10);
    strcpy(s1, "Hello");
    strcpy(s2, "Hello");
    strcpy(s3, "Hello");
    strcpy(s4, "Hello");
    t1->data = 1;
    t2->data = 2;
    t3->data = 3;
    t4->data = 4;
    t1->next = t2;
    t2->next = t3;
    t3->next = t4;
    t4->next = NULL;
    print_reverse_list(t1);
    printf("\n");
}
