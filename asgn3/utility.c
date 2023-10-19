#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "utility.h"

#define NUM_BYTES 256
#define BUFFERSIZE 100

int *histogram(int input_fd)
{
    /* initialize histogram with size of 256 (max number of different bytes)*/
    int *histogram = (int *)calloc(NUM_BYTES, sizeof(int));
    if (!histogram)
    {
        exit(EXIT_FAILURE);
    }

    /* loop through input and count occurance*/
    ssize_t bytes_read;
    int i;
    uint8_t buffer[BUFFERSIZE];
    while ((bytes_read = read(input_fd, buffer, BUFFERSIZE)))
    {
        /* handle error while reading*/
        if (bytes_read == -1)
        {
            perror("Reading Error");
            exit(EXIT_FAILURE);
        }
        /* loop through buffered variable to fill histogram*/
        for (i = 0; i < bytes_read; i++)
        {
            *(histogram + (buffer[i])) += 1;
        }
    }

    /* set offset back to 0 to reread input for encoding*/
    if (lseek(input_fd, 0, SEEK_SET) == -1)
    {
        perror("Offset Error");
        exit(EXIT_FAILURE);
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
    new_node->left_ch = NULL;
    new_node->right_ch = NULL;
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
        /* tiebreaker convention automatically applies, because my histogram is
         * sorted in ascending byte order*/
        while ((current != NULL) && ((current->freq) <= (new_node->freq)))
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

node *linkedList(int *histogram)
{
    /* initialize an empty sorted linked list*/
    node *head = NULL;

    /* traverse the histogram array and create nodes for non-zero frequency elements*/
    int i;
    for (i = 0; i < NUM_BYTES; i++)
    {
        if (histogram[i] > 0)
        {
            node *new_node = createNode((char)i, histogram[i]);
            /* Insert the node into the sorted linked list*/
            head = insertSorted(head, new_node);
        }
    }

    return head;
}

void printList(node *head)
{
    node *current = head;
    while (current != NULL)
    {
        printf("Byte: %d, Frequency: %d\n", current->byte, current->freq);
        current = current->next;
    }
}

void freeList(node *head)
{
    /* free all allocated memory for the nodes*/
    node *previous = head;
    node *current = head;

    /* loop through linked list and free each node memory*/
    while (current)
    {
        previous = current;
        current = previous->next;
        free(previous);
    }
}

node *binaryTree(node *head)
{
    /* initialize variables to handle binary tree creation*/
    node *merge;
    node *previous = NULL;
    node *current = head;

    /* loop through linked list and create binary tree accordingly*/
    while (current->next)
    {
        /* as linked list is sorted take first two elements*/
        previous = current;
        current = previous->next;

        /* combine first two elements into merged node and assign the nodes as
         * children*/
        merge = createNode(0, (current->freq + previous->freq));
        merge->left_ch = previous;
        merge->right_ch = current;

        /* insert merged node back into link list and get head of linked list*/
        current = insertSorted(current->next, merge);

        /* break connection of child nodes to linked list*/
        merge->left_ch->next = NULL;
        merge->right_ch->next = NULL;
    }

    return current;
}

void printBinaryTree(node *root)
{
    if (root == NULL)
        return;

    /* First recur on left child*/
    printBinaryTree(root->left_ch);

    /* only print leaves*/
    if (!root->left_ch && !root->right_ch)
        printf("Nodedata: %d Nodefreq: %i\n", root->byte, root->freq);

    /* Now recur on right child*/
    printBinaryTree(root->right_ch);
}

void populateHTable(node *root, h_table_entry **h_table, char *path, int *index)
{
    if (root == NULL)
    {
        return;
    }
    /* check if current node is a leaf and if so add to hash table*/
    if (!root->left_ch && !root->right_ch)
    {
        h_table[*index]->byte = root->byte;
        strcpy(h_table[*index]->encoding, path);
        (*index)++;
    }

    /* else traverse further into tree*/
    /* creating two new path variables for new path to go*/
    char leftPath[256], rightPath[256];
    strcpy(leftPath, path);
    strcpy(rightPath, path);
    strcat(leftPath, "0");
    strcat(rightPath, "1");
    populateHTable(root->left_ch, h_table, leftPath, index);
    populateHTable(root->right_ch, h_table, rightPath, index);
}

void hTableSort(h_table_entry **h_table, int num_entries)
{
    int swapped;
    int i;
    do
    {
        swapped = 0;
        for (i = 1; i < num_entries; i++)
        {
            if ((h_table[i - 1]->byte) > (h_table[i]->byte))
            {

                /* Swap h_table[i-1] and h_table[i]*/
                h_table_entry *temp = h_table[i - 1];
                h_table[i - 1] = h_table[i];
                h_table[i] = temp;

                /* Set the flag to true to continue sorting*/
                swapped = 1;
            }
        }
    } while (swapped);
}

void applyHeader(int output_fd, h_table_entry **h_table, int num_entries)
{
    return;
}

void applyEncoding(int input_fd, int output_fd, h_table_entry **h_table, int num_entries)
{
    int c_index;
    int e_index;
    int d_index;

    char bit;

    int bits_count = 0;
    uint8_t encoding_byte = 0;

    ssize_t bytes_read;
    ssize_t bytes_written;
    uint8_t buffer[BUFFERSIZE];
    while ((bytes_read = read(input_fd, buffer, BUFFERSIZE)))
    {
        /* handle error while reading*/
        if (bytes_read == -1)
        {
            perror("Reading Error");
            exit(EXIT_FAILURE);
        }
        /* loop through buffered variable handle each byte*/
        for (c_index = 0; c_index < bytes_read; c_index++)
        {
            for (e_index = 0; e_index < num_entries; e_index++)
            {
                if (h_table[e_index]->byte == buffer[c_index])
                {
                    for (d_index = 0; (bit = h_table[e_index]->encoding[d_index]); d_index++)
                    {
                        if (bit == '0')
                        {
                            /* bitshift to fil in byte with encoding*/
                            encoding_byte = (encoding_byte << 1) & 0xFE;
                        }
                        else
                        {
                            /* bitshift to fil in byte with encoding*/
                            encoding_byte = (encoding_byte << 1) | 0x01;
                        }

                        bits_count++;
                        /* if 1 byte is full write it*/
                        if (bits_count == 8)
                        {
                            /* write the generated encoding byte to the output file*/
                            bytes_written = write(output_fd, &encoding_byte, 1);
                            if (bytes_written == -1)
                            {
                                perror("Writing Error");
                                exit(EXIT_FAILURE);
                            }

                            /* reset bitcounter as well as encoding byte for next byte*/
                            bits_count = 0;
                            encoding_byte = 0;
                        }
                    }
                }
            }
        }
        
        /* pad remaining byte*/
        if (bits_count != 0)
        {
            /* fill in rest of the byte with 0*/
            int fil_index;
            for(fil_index = 8-bits_count; fil_index < 8; fil_index++)
            {
                 encoding_byte = (encoding_byte << 1) & 0xFE;
            }
            /* write the generated encoding byte to the output file*/
            bytes_written = write(output_fd, &encoding_byte, 1);
            if (bytes_written == -1)
            {
                perror("Writing Error");
                exit(EXIT_FAILURE);
            }
        }
    }
    return;
}