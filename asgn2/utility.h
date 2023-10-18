#ifndef utility_h
#define utility_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* This function takes in a FILE Pointer to a binary file and creates a
 * histogram for the bytes in that file.
 * Arguments: file pointer
 * Return: histogram array*/
int *histogram(FILE *file);

/* node struct for linked list which stores the freq of character and pointer to
 * next node in linked list 
 *
 * Attributes: 
 * freq = number of occurences
 * byte = value of the byte
 * *next = pointer to next element in linked list*/
struct node_st {
    int freq;
    char byte;
    struct node_st *next;
};
typedef struct node_st node;

/* Helper function to creat a node and return the pointer to that new node
 * Arguments: byte-value and frequencie
 * Return: pointer to new node*/
node* createNode(char byte, int freq);

/* Helper function to insert a node into the linked list while maintaining
 * sorted order
 * Arguments: head of linked list and new_node
 * Return: pointer to head of linked list*/
node *insertSorted(node* head, node* new_node);

/* This function recieves a histogram and returns the first element (least
 * frequent) of a sorted linked list.
 * Arguments: histogram array
 * Return: pointer to first element (least frequent) in sorted linked list*/
node *linked_list(int* histogram);

/* Simple helper function for debugging that prints out every node in linked
 * list
 * Arguments: head of linked list*/
void printList(node* head);


/* This function iterates through the linked list and frees all alllocated
 * memory
 * Arguments: pointer to start of linked list*/
void free_list(node *head);

#endif