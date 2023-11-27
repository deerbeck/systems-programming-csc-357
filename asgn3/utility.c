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
        perror("malloc");
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

node *createNode(int byte, int freq)
{
    /* allocate memory for new node*/
    node *new_node = (node *)malloc(sizeof(node));
    if (new_node == NULL)
    {
        perror("malloc");
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
    node *temp_node;

    /* if list is empty or heads frequency is bigger start with new_node as
     * head*/
    /* TIEBRAKER CONVENTION needs to be applied to the head aswell......*/
    if (!head || head->freq > new_node->freq ||
        (head->freq == new_node->freq && head->byte >= new_node->byte))
    {
        new_node->next = head;
        head = new_node;
    }
    else
    {
        /* first sort for frequency*/
        for (temp_node = head;
             temp_node->next && temp_node->next->freq < new_node->freq;
             temp_node = temp_node->next)
            ;
        /* wheee hopefully*/

        /* now sort for bytes*/

        for (temp_node = temp_node;
             temp_node->next && temp_node->next->byte < new_node->byte &&
             temp_node->next->freq == new_node->freq;
             temp_node = temp_node->next)
            ;
        /* wheee again*/

        /* finaly do the insert*/
        new_node->next = temp_node->next;
        temp_node->next = new_node;
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
            node *new_node = createNode(i, histogram[i]);
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
        printf("Byte: %d, Frequency: %d\n", (uint8_t)current->byte,
               current->freq);
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
        /* Magic Number "-1" is used to give merged number a smaller value for
         * Tiebraker that is not a valid byte in the file*/
        merge = createNode((-1), (current->freq + previous->freq));
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
        /* typcast to uint8_t for beacuse int was only used to sort in merged
         * nodes*/
        h_table[*index]->byte = (uint8_t)root->byte;
        strcpy(h_table[*index]->encoding, path);
        (*index)++;
    }

    /* else traverse further into tree*/
    /* creating two new path variables for new path to go*/
    char leftPath[NUM_POSSIB_BYTES], rightPath[NUM_POSSIB_BYTES];
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

void writeHeader(int output_fd, int *histogram, uint16_t num_entries)
{

    /* sizes of header properties are given in specification:
     * 1 byte for num
     * then for each byte = 1 byte and frequency = 4 bytes*/
    /* write num to header*/
    int bytes_written;
    /* num is 1 smaller to fit in a byte*/
    uint8_t num = num_entries - 1;
    /* write only 1 byte to header (num of entries)*/
    bytes_written = write(output_fd, &num, 1);
    if (bytes_written == -1)
    {
        perror("Writing Error");
        exit(EXIT_FAILURE);
    }

    /* write byte and frequency*/
    /* this chunk is not as efficient as possible because of writing single
     * bytes but it is only for the header (max of 256 entries)*/
    int i;
    uint8_t byte;
    uint32_t frequency;
    for (i = 0; i < NUM_POSSIB_BYTES; i++)
    {
        if (histogram[i] > 0)
        {
            /* get byte value represented as index and convert back to uchar*/
            byte = (uint8_t)i;
            /* write only 1 byte (size of character)*/
            bytes_written = write(output_fd, &byte, 1);
            if (bytes_written == -1)
            {
                perror("Writing Error");
                exit(EXIT_FAILURE);
            }

            /* convert frequency to network byte order*/
            frequency = htonl(histogram[i]);
            /* write 4 bytes (size of frequency as specified in
             * specification)*/
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
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* initialize data array with set arbitrary size*/
    bs->data = (uint8_t *)malloc(BITSTREAM_SIZE);
    if (bs->data == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* set newly allocated memory to 0. This takes care of the needed
     * padding if last byte won't be filled completely*/
    /* I know I don't have to set my whole bitstream to 0, but this makes it
     * easy and convinient to take care of in 1 go while i'm at initializing*/
    int i;
    for (i = 0; i < BITSTREAM_SIZE; i++)
    {
        bs->data[i] = 0;
    }
    /* adjust bitstream size*/
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
            /* needed to free memory if allocation failed*/
            free(bs->data);
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        bs->data = tmp;

        /* set newly allocated memory to 0. This takes care of the needed
         * padding if last byte won't be filled completely*/
        /* I know I don't have to set my whole bitstream to 0, but this makes
         * it easy and convinient to take care of in 1 go while i'm at
         * initializing*/
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
    if (bit)
    {
        /* set bit to 1*/
        bs->data[byteIndex] |= (1 << (7 - bitOffset));
        /* bit only needs to be set, becuase bitstreams bytes are all
         * initialized to 0*/
    }

    /* increment index*/
    bs->index++;
}

void generateEncoding(int input_fd, int output_fd, h_table_entry **h_lookup,
                      int num_entries, bitstream *bs)
{
    /* index variables ->c_index = current to read character position and
     * d_index = current encoding position*/
    int c_index;
    int d_index;

    /* current bit to write*/
    unsigned char bit;
    ssize_t bytes_read;
    uint8_t buffer[READ_WRITE_BUFFER_SIZE];

    /* while not reaching EOF keep reading*/
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
            /* loop though the encoding of provided character*/
            /* encoding is located at index = character*/
            for (d_index = 0;
                 (bit = h_lookup[buffer[c_index]]->encoding[d_index]);
                 d_index++)
            {
                /* you can skip 0 in theory because bitstream is initialized to
                0, but this makes it more readeable for me*/
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
    /* get the nuber of actual bytes to write to file
     * (bs->index+8-1) / 8 this round up to the next integer*/
    size_t bytes_to_write = ((bs->index + 8 - 1) / 8);

    /* write and handle error*/
    if (write(output_fd, bs->data, bytes_to_write) == -1)
    {
        perror("Writing error");
        exit(EXIT_FAILURE);
    }
}

node *readHeader(int input_fd)
{
    node *head = NULL;
    int i;
    uint16_t num;
    uint8_t byte;
    uint32_t frequency;
    ssize_t bytes_read;

    /* read number of different bytes and handle error*/
    bytes_read = read(input_fd, &num, 1);
    if (bytes_read == -1)
    {
        perror("Reading Error");
        exit(EXIT_FAILURE);
    }
    /* file is empty*/
    else if (bytes_read == 0)
    {
        return head;
    }

    /* number is actually 1 bigger*/
    num++;

    /* this chunk is not as efficient as possible because of reading single
     * bytes but it is only for the header (max of 256 entries)*/
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

        /* create new node corresponding to the read byte and frequency and
        insert into linked List accordingly*/
        node *new_node = createNode(byte, frequency);
        head = insertSorted(head, new_node);
    }
    /* return head of the linked list*/
    return head;
}

ssize_t readBody(int input_fd, bitstream *bs)
{
    /* variable to return later*/
    ssize_t bytes_read;

    /* UNIX IO read into the bitstream data*/
    bytes_read = read(input_fd, bs->data, BITSTREAM_SIZE);

    /* handle reading error*/
    if (bytes_read == -1)
    {
        perror("Reading Error");
        exit(EXIT_FAILURE);
    }
    /* reset Bitstream index*/
    bs->index = 0;

    return bytes_read;
}

void decodeBody(int input_fd, int output_fd, node *root, bitstream *bs,
                uint32_t total_bytes)
{
    /* num of bytes that have been read by UNIX IO read()*/
    ssize_t bytes_read;
    /* start node is the root of the binary tree*/
    node *current = root;
    /* byte_count to keep track of bytes to be written*/
    uint32_t byte_count = 0;
    /* helper variables for reading right bit*/
    int byteIndex;
    int bitOffset;

    /* buffer to store bytes in and write to file - initialize to 0*/
    uint8_t buffer[READ_WRITE_BUFFER_SIZE];
    int i;

    /* first read data into bitstream data*/
    bytes_read = readBody(input_fd, bs);

    /* loop until number of total_bytes is reached*/
    while (byte_count < (total_bytes))
    {
        /* bytes read * 8 == bits read*/
        if (bs->index >= (bytes_read * 8))
        {
            /* get new data if end of buffer is reached*/
            bytes_read = readBody(input_fd, bs);
        }

        /* check if we can still traverse to childs*/
        if (current->left_ch && current->right_ch)
        {
            /* current byte we are working on*/
            byteIndex = bs->index / 8;

            /* current bit in byte to be checked 8- (indx+1) 8- because of
             * order of bits (left to right given through encoding) and +1
             * because for bit operation the index does not start at 0 but
             * at 1*/
            bitOffset = (8 - ((bs->index % 8) + 1));
            /* increment index to bit*/

            bs->index++;
            /* check if bit at offset location is 1 and traverse tree to
             * right child accordingly*/
            if (bs->data[byteIndex] & (1 << bitOffset))
            {
                current = current->right_ch;
            }
            /* else traverse to the left_child*/
            else
            {
                current = current->left_ch;
            }
        }
        /* leaf is reached*/
        else
        {

            /* add byte to buffer*/
            buffer[byte_count % READ_WRITE_BUFFER_SIZE] = current->byte;

            /* check if buffer is full index is 1 less than size*/
            if (((byte_count + 1) % (READ_WRITE_BUFFER_SIZE)) == 0)
            {

                /* write buffer to output and check error*/
                /* size to write modulo READ_WRITE_BUFFER_SIZE +1 because index
                is 1 less than size*/
                if (write(output_fd, buffer,
                          (byte_count % READ_WRITE_BUFFER_SIZE) + 1) == -1)
                {

                    perror("Writing Error");
                    exit(EXIT_FAILURE);
                }
                /* reset values in buffer*/
                for (i = 0; i < READ_WRITE_BUFFER_SIZE; i++)
                {
                    buffer[i] = 0;
                }
            }
            /* increase byte_count to keep track at when to stop*/
            byte_count++;

            /* reset current position in tree back to root*/
            current = root;
        }
    }
    /* write remaining contents of buffer*/
    /* adding of +1 is no longer needed because byte_count got incremented
     * 1 more time*/
    if (write(output_fd, buffer, (byte_count % READ_WRITE_BUFFER_SIZE)) == -1)
    {

        perror("Writing Error");
        exit(EXIT_FAILURE);
    }
}
