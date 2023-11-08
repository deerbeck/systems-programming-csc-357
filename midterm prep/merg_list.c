#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
typedef struct node_st
{
    int data;
    struct node_st *next;
} Node;

Node *merge_lists(Node *a, Node*b)
{
   if(!a) return b;
   if(!b) return a;
   Node *tmp;
   Node *head;
   
   if(a->data <= b->data)
   {
    head = a;
    tmp = head;
    a = a->next;
   }
   else{
    head = b;
    tmp = head;
    b = b->next;
   }

   while(a || b)
   {
    if(!b || (a && a->data <= b->data))
    {
        tmp->next = a;
        a = a->next;
        tmp = tmp->next;
    }
    else if(!a || (b && b->data <= a->data))
    {
        tmp->next = b;
        b = b->next;
        tmp = tmp->next;
    }
   }
   return head;
}

int main(int argc, char *argv[])
{
    struct node_st *t1 = (struct node_st *)malloc(sizeof(struct node_st));
    struct node_st *t2 = (struct node_st *)malloc(sizeof(struct node_st));
    struct node_st *t3 = (struct node_st *)malloc(sizeof(struct node_st));
    struct node_st *t4 = (struct node_st *)malloc(sizeof(struct node_st));
    t1->data = 2;
    t2->data = 4;
    t3->data = 6;
    t4->data = 8;
    t1->next = t2;
    t2->next = t3;
    t3->next = t4;
    t4->next = NULL;

    struct node_st *v1 = (struct node_st *)malloc(sizeof(struct node_st));
    struct node_st *v2 = (struct node_st *)malloc(sizeof(struct node_st));
    struct node_st *v3 = (struct node_st *)malloc(sizeof(struct node_st));
    struct node_st *v4 = (struct node_st *)malloc(sizeof(struct node_st));
    v1->data = 1;
    v2->data = 3;
    v3->data = 5;
    v4->data = 7;
    v1->next = v2;
    v2->next = v3;
    v3->next = v4;
    v4->next = NULL;
    Node *test = merge_lists(NULL, t1);
    printf("\n");
}
