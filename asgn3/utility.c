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
        while ((current != NULL))
        {
            if (current->freq < new_node->freq)
            {
                previous = current;
                current = previous->next;
            }
            else if ((current->freq == new_node->freq) &&
                     ((current->byte < new_node->byte)))
            {
                previous = current;
                current = previous->next;
            }
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
    /* if leaf is reached free that leaf and set to null*/
    free(root);
    root = NULL;
    /* so parents become new leaf*/
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

void writeHeader(int output_fd, int *histogram, int num_entries)
{
    /* write num to header*/
    int bytes_written;
    uint8_t num = num_entries - 1;
    bytes_written = write(output_fd, &num, 1);
    if (bytes_written == -1)
    {
        perror("Writing Error");
        exit(EXIT_FAILURE);
    }

    /* write byte and frequency*/
    int i;
    uint8_t byte;
    uint32_t frequency;
    for (i = 0; i < NUM_POSSIB_BYTES; i++)
    {
        if (histogram[i] > 0)
        {
            /* get byte value represented as index and convert back to uchar*/
            byte = (uint8_t)i;
            bytes_written = write(output_fd, &byte, 1);
            if (bytes_written == -1)
            {
                perror("Writing Error");
                exit(EXIT_FAILURE);
            }

            /* convert frequency to network byte order*/
            frequency = htonl(histogram[i]);
            bytes_written = write(output_fd, &frequency, 4);
            if (bytes_written == -1)
            {
                perror("Writing Error");
                exit(EXIT_FAILURE);
            }
        }
    }
    return;
}

bitstream *createBitstream()
{
    /* creaate bitstream struct*/
    bitstream *bs = malloc(sizeof(bitstream));
    /* check for memory allocation error*/
    if (bs == NULL)
    {
        exit(EXIT_FAILURE);
    }
    /* initialize data array with set arbitrary size*/
    bs->data = (uint8_t *)malloc(BITSTREAM_SIZE);
    /* set all of the data to 0*/
    int i;
    for (i = 0; i < BITSTREAM_SIZE; i++)
    {
        bs->data[i] = 0;
    }
    bs->size = BITSTREAM_SIZE;
    /* set current cursor index to 0*/
    bs->index = 0;
    return bs;
}

void writeBitBitstream(bitstream *bs, uint8_t bit)
{
    /* handling overflow by resizing data array*/
    if (bs->index / 8 >= bs->size)
    {
        /* resize bitstream data and handle any occuring error*/
        uint8_t *tmp;
        bs->size += BITSTREAM_SIZE;
        tmp = realloc(bs->data, bs->size);
        if (tmp == NULL)
        {
            free(bs->data);
            exit(EXIT_FAILURE);
        }
        bs->data = tmp;
        /* set newly allocated memory to 0*/
        int i;
        for (i = (bs->index / 8); i < bs->size; i++)
        {
            bs->data[i] = 0;
        }
    }
    /* helper variables to set bit*/
    int byteIndex = bs->index / 8;
    int bitOffset = bs->index % 8;

    /* set the bit at the current index*/
    if (!bit)
    {
        bs->data[byteIndex] &= ~(0 << (7 - bitOffset));
    }
    else
    {
        bs->data[byteIndex] |= (1 << (7 - bitOffset));
    }

    /* increment index*/
    bs->index++;
}

void generateEncoding(int input_fd, int output_fd, h_table_entry **h_lookup,
                      int num_entries, bitstream *bs)
{
    int c_index;
    int d_index;

    char bit;

    ssize_t bytes_read;
    uint8_t buffer[READ_WRITE_BUFFER_SIZE];
    while ((bytes_read = read(input_fd, buffer, READ_WRITE_BUFFER_SIZE)))
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
            /* loop though the encoding of provided character and */
            /* encoding is located at index = character*/
            for (d_index = 0;
                 (bit = h_lookup[buffer[c_index]]->encoding[d_index]);
                 d_index++)
            {
                if (bit == '0')
                {
                    /* write 0 bit to bitstream*/
                    writeBitBitstream(bs, 0);
                }
                else
                {
                    /* write 1 bit to bitstream*/
                    writeBitBitstream(bs, 1);
                }
            }
        }
    }
}

void writeEncoding(int output_fd, bitstream *bs)
{
    /* create buffer to write from to file*/
    uint8_t *buffer = (uint8_t *)malloc((bs->index / 8));
    if (buffer == NULL)
    {
        exit(EXIT_FAILURE);
    }

    /* memcopy to buffer only the bytes that need to be written.*/
    /*(bs->index+8-1) / 8 this round up to the next integer*/
    size_t bytes_to_write = ((bs->index + 8 - 1) / 8);
    memcpy(buffer, bs->data, bytes_to_write);

    if (write(output_fd, buffer, bytes_to_write) == -1)
    {
        perror("Writing error");
        exit(EXIT_FAILURE);
    }
    /* free buffer again*/
    free(buffer);
}

node *readHeader(int input_fd)
{
    node *head = NULL;
    int i;
    uint8_t num;
    uint8_t byte;
    uint32_t frequency;

    /* read number of different bytes and handle error*/
    if (read(input_fd, &num, 1) == -1)
    {
        perror("Reading Error");
        exit(EXIT_FAILURE);
    }

    /* number is actually 1 bigger*/
    num++;

    for (i = 0; i < num; i++)
    {
        /* read byte value and handle error*/
        if (read(input_fd, &byte, 1) == -1)
        {
            perror("Reading Error");
            exit(EXIT_FAILURE);
        }

        if (read(input_fd, &frequency, 4) == -1)
        {
            perror("Reading Error");
            exit(EXIT_FAILURE);
        }

        /* convert frequency back to hostbyte order*/
        frequency = ntohl(frequency);

        node *new_node = createNode(byte, frequency);

        head = insertSorted(head, new_node);
    }

    return head;
}

void decodeBody(int input_fd, int output_fd, node *root, bitstream *bs)
{
    ssize_t bytes_read;
    node *current = root;

    /* helper variables for reading right bit*/
    int byteIndex;
    int bitOffset;

    /* read into buffer the size of the bitstream array*/
    while ((bytes_read = read(input_fd, bs->data, BITSTREAM_SIZE)))
    {
        bs->index = 0;
        /* check for reading error*/
        if (bytes_read == -1)
        {
            perror("Reading Error");
            exit(EXIT_FAILURE);
        }
        while (bs->index < ((bytes_read * 8) + 1))
        {

            if (current->left_ch && current->right_ch)
            {
                /* current byte we are working on*/
                byteIndex = bs->index / 8;
                /* current bit in byte to be checked 8- (indx+1) 8- because of
                 * order of bits (left to right given through encodeng) and +1
                 * because for bit operation the index does not start at 0 but
                 * at 1*/
                bitOffset = (8 - ((bs->index % 8) + 1));
                bs->index++;
                /* check if bit at offset location is 1 and traverse tree to
                 * right child accordingly*/
                if (bs->data[byteIndex] & (1 << bitOffset))
                {
                    current = current->right_ch;
                }
                else
                {
                    current = current->left_ch;
                }
            }
            else
            {
                /* write character to output and check error*/
                if ((write(output_fd, &(current->byte), 1)) == -1)
                {

                    perror("Writing Error");
                    exit(EXIT_FAILURE);
                }
                /* reset current position in tree back to root*/
                current = root;
            }
        }
    }
}