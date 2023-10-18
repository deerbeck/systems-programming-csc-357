#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utility.h"

#define NUM_BYTES 256

int *histogram(FILE *file)
{
    /* initialize histogram with size of 256 (max number of different bytes)*/
    int *histogram = (int *)calloc(NUM_BYTES, sizeof(int));
    if (!histogram)
    {
        exit(EXIT_FAILURE);
    }

    /* loop through input and count occurance*/
    char c;
    while ((c = fgetc(file)) != EOF)
    {
        *(histogram + c) += 1;
    }

    return histogram;
}

node *createNode(char byte, int freq)
{
    /* allocate memory for new node*/
    node *new_node = (node *)malloc(sizeof(node));
    if (new_node == NULL)
    {
        exit(EXIT_FAILURE);
    }

    /* initialize new Node with given values*/
    new_node->freq = freq;
    new_node->byte = byte;
    new_node->next = NULL;

    return new_node;
}

node *insertSorted(node *head, node *new_node)
{
    /* create buffer nodes*/
    node *previous;
    node *current;


    /* if list is empty start with new_node as head*/
    if (!head)
    {
        head = new_node;
    }
    /* switch head to new_node, if frequency is smaller than head*/
    else if (head->freq > new_node->freq)
    {
        new_node->next = head;
        return new_node;
    }
    /* if new_node frequency is bigger than head frequency iterate through the
     * list and insert in the right place*/
    else
    {
        previous = NULL;
        current = head;
        /* iterate through nodes until the spot between nodes is found*/
        while ((current != NULL) && ((current->freq) < (new_node->freq)))
        {
            previous = current;
            current = previous->next;
        }

        /* swap nodes and insert new node*/
        previous->next = new_node;
        new_node->next = current;
    }

    return head;
}

node *linked_list(int *histogram)
{
    /* initialize an empty sorted linked list*/
    node *head = NULL;

    /* traverse the histogram array and create nodes for non-zero frequency elements*/
    int i;
    for (i = 0; i < NUM_BYTES; i++)
    {
        if (histogram[i] > 0)
        {
            node *new_node = createNode((char) i, histogram[i]);
            /* Insert the node into the sorted linked list*/
            head = insertSorted(head, new_node);
        }
    }

    return head;
}

void printList(node* head) {
    node* current = head;
    while (current != NULL) {
        printf("Byte: %d, Frequency: %d\n", current->byte, current->freq);
        current = current->next;
    }
}


void free_list(node *head)
{
    node *previous = head;
    node *current = head;
    while(current) {
        previous = current;
        current = previous->next;
        free(previous);
    }       
}
