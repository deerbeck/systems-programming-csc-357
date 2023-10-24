/***Written by Johannes Hirschbeck (jhirsc01). The functions provided here are
 * used to propperly encode and decoda a file using the huffman encoding.
 ***/

#include "utility.h"

int *histogram(int input_fd)
{
    /* initialize histogram with size of 256 (max number of different bytes)*/
    int *histogram = (int *)calloc(NUM_POSSIB_BYTES, sizeof(int));
    if (!histogram)
    {
        exit(EXIT_FAILURE);
    }

    /* loop through input and count occurance*/
    ssize_t bytes_read;
    int i;
    uint8_t buffer[READ_WRITE_BUFFER_SIZE];
    while ((bytes_read = read(input_fd, buffer, READ_WRITE_BUFFER_SIZE)))
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
    /* create buffer node pointers*/
    node *previous;
    node *current;

    /* if list is empty start with new_node as head*/
    if (head == NULL)
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
        if (current->freq == new_node->freq && new_node->byte < current->byte)
        {
            new_node->next = current;
            return new_node;
        }
        /* iterate through nodes until the spot between nodes is found if node
        is null insert new node at the end of linked list*/
        while ((current != NULL))
        {
            /* continue going through linked list if frequency is still bigger*/
            if (current->freq < new_node->freq)
            {
                previous = current;
                current = previous->next;
            }
            /* if frequenccy is the same apply tiebraker convention and insert
             * according to the byte avlue*/
            else if ((current->freq == new_node->freq) &&
                     ((current->byte < new_node->byte)))
            {
                previous = current;
                current = previous->next;
            }
            /* stop if spot inside of linked list is found*/
            else
            {
                break;
            }
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

    /* traverse the histogram array and create nodes for non-zero frequency
     * elements*/
    int i;
    for (i = 0; i < NUM_POSSIB_BYTES; i++)
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

void freeBinaryTree(node *root)
{
    if (root->left_ch && root->right_ch)
    {
        /* travers down tree recursively*/
        freeBinaryTree(root->left_ch);
        freeBinaryTree(root->right_ch);
    }
    /* if leaf is reached free that leaf and set to null so parents become new
    leaf*/
    free(root);
    root = NULL;
    return;
}

void populateHTable(node *root, h_table_entry **h_table,
                    char *path, int *index)
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

int compareEnntries(const void *entry_a, const void *entry_b)
{
    /* my h_table array is an array of pointers which point to my entry structs
    this is to get the pointer to that entry struct and keeping it constant*/
    const h_table_entry *const(*entry_a_sort) = entry_a;
    const h_table_entry *const(*entry_b_sort) = entry_b;
    /* comparing the bytes to sort them in ascending order*/
    if ((*entry_a_sort)->byte < (*entry_b_sort)->byte)
    {
        return -1;
    }
    else if ((*entry_a_sort)->byte > (*entry_b_sort)->byte)
    {
        return 1;
    }

    return 0;
}