#ifndef utility_h
#define utility_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define NUM_POSSIB_BYTES 256
#define READ_WRITE_BUFFER_SIZE 4096
#define BITSTREAM_SIZE 4096

/* This function takes in a FILE Pointer to a binary file and creates a
 * histogram for the bytes in that file.
 * Arguments: file pointer
 * Return: histogram array*/
int *histogram(int fd);

/* node struct that will be used for the linked list aw well as the binary tree
 * strucutre. It which stores the freq of character and pointer to next node in
 * linked list as well as its childs.
 * Attributes:
 * freq = number of occurences
 * byte = value of the byte
 * *left_child = pointer to left (lesser frequency) child
 * *right_child = pointer to right (higher frequency) child
 * *next = pointer to next element in linked list*/
struct node_st
{
    int freq;
    unsigned char byte;
    struct node_st *left_ch;
    struct node_st *right_ch;
    struct node_st *next;
};
typedef struct node_st node;

/* Helper function to create a node and return the pointer to that new node
 * Arguments: byte-value and frequencie
 * Return: pointer to new node*/
node *createNode(char byte, int freq);

/* Helper function to insert a node into the linked list while maintaining
 * sorted order
 * Arguments: head of linked list and new_node
 * Return: pointer to head of linked list*/
node *insertSorted(node *head, node *new_node);

/* This function recieves a histogram and returns the first element (least
 * frequent) of a sorted linked list.
 * Arguments: histogram array
 * Return: pointer to first element (least frequent) in sorted linked list*/
node *linkedList(int *histogram);

/* Simple helper function for debugging that prints out every node in linked
 * list
 * Arguments: head of linked list*/
void printList(node *head);

/* This function iterates through the linked list and frees all nodes
 * memory
 * Arguments: pointer to start of linked list*/
void freeList(node *head);

/* This function recieves the head of a linked list and turns it into a binary
 * tree representing the huffman encoding.
 * Arguments: head of the linked list
 * Return: pointer to the root node of the binary tree*/
node *binaryTree(node *head);

/* Simple helper function for debugging that prints out every node in the
 * binary tree.
 * Arguments: root of binary tree*/
void printBinaryTree(node *root);

/* This function iterates through the binar Tree and frees the nodes and their
 * allocated memory
 * Arguments: pointer to start of linked list*/
void freeBinaryTree(node *root);

/* Structure to store the encoding for each byte in the binary tree
 * Attributes:
 * byte = value of the byte
 * endocing = array of the bits stored as characters (resulting in a string)
 * (NUM_POSSIBLE_BYTES is the longest possible bit sequence)*/
struct h_table_entry
{
    uint8_t byte;
    char encoding[NUM_POSSIB_BYTES];
};
typedef struct h_table_entry h_table_entry;

/* This function pupulates a hash table by recursevly stepping down each
 * path of a binary tree, recording its path on the way
 * Arguments:
 * root (current node pointer)
 * hash table as an array which contains structures of the entries
 * path to the entrie of the binary tree stored in a string
 * current index in which the entry is going to be stored*/
void populateHTable(node *root, h_table_entry **h_table,
                    char *path, int *index);

/* This function exists to be used in the qsort algorithm to sort the h_table
 * according to the byte values.*/
int compareEnntries(const void *entry_a, const void *entry_b);

/* This function takes the histogram which stores the frequencies of each
 * byte and
 * writes it to the provided output stream
 * Arguments:
 * output_fd file descriptor for output stream
 * histogram array
 * number of different bytes found in input file*/
void writeHeader(int output_fd, int *histogram, uint16_t num_entries);

/* Structure to store the encoding as a bitstream.
 * Attributes:
 * data pointer to byte array which stores the data
 * current size of the data in bytes
 * current index of the bitstream*/
struct bitstream_st
{
    uint8_t *data;
    int size;
    int index;
};
typedef struct bitstream_st bitstream;

/* Helper function to initialize the bitstream and store some initail data
 * in it.
 * Arguments:
 * Return pointer to initialized bitstream struct*/
bitstream *createBitstream();

/* Write a single bit to the bitstream and resize if needed
 * Arguments:
 * bs bitstream pointer
 * bit the value of the bit to set*/
void writeBitBitstream(bitstream *bs, uint8_t bit);

/* This function encodes a provided input file using a hash table which stores
 * the encoding of each byte and writes it to the output stream
 * Arguments:
 * inputt_fd file descriptor for input stream
 * output_fd file descriptor for output stream
 * hash table (array of hash table structs)
 * number of different bytes found in input file
 * bitstream pointer to the bitstream*/
void generateEncoding(int input_fd, int output_fd, h_table_entry **h_table,
                      int num_entries, bitstream *bs);

/* This function writes the encoding stored in the bitstream to the output
 * filestream
 * Arguments:
 * output_fd file descriptor for output stream
 * bitstream pointer to the bitstream*/
void writeEncoding(int output_fd, bitstream *bs);

/* This function takes the input file descriptor, reads in the header and
 * generates a corresponding linked List out of it.
 * Arguments:
 * input_fd file descriptor for input stream
 * linked List genereated with given input file*/
node *readHeader(int input_fd);

/* This takes the input file and the pointer to the bitstream and reads into
 * the bitstream data array. And returns the number of the actual bytes read.
 * Arguments:
 * input_fd file descriptor for input stream
 * bistream pointer to bitstream
 * Return:
 * number of read bytes*/
ssize_t readBody(int input_fd, bitstream *bs);

/* This function takes the input file and output file as well as the binary
 * tree for decoding and decodes the input file by traversing down the
 * binary tree and writing the decompressed data to the output file.
 * Arguments:
 * input_fd file descriptor for input stream
 * output_fd file descriptor for input stream
 * root of binary Tree
 * bistream pointer to bitstream*/
void decodeBody(int input_fd, int output_fd, node *root, bitstream *bs,
                uint32_t total_bytes);

#endif