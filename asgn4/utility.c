#include "utility.h"

/* initialize global variables*/
int verbose = 0;
int create = 0;
int listing = 0;
int extract = 0;
int strict = 0;
int file = 0;

uint32_t extract_special_int(char *where, int len)
{
    int32_t val = -1;
    if ((len >= sizeof(val)) && (where[0] & 0x80))
    {
        /* the top bit is set and we have space
         * extract the last four bytes */
        val = *(int32_t *)(where + len - sizeof(val));
        val = ntohl(val);
        /* convert to host byte order */
    }
    return val;
}

int insert_special_int(char *where, size_t size, int32_t val)
{

    int err = 0;
    if (val < 0 || (size < sizeof(val)))
    {
        /* if it’s negative, bit 31 is set and we can’t use the ﬂag
         * if len is too small, we can’t write it. Either way, we’re
         * done.
         */
        err++;
    }
    else
    {
        /* game on....*/
        memset(where, 0, size);
        /* Clear out the buﬀer */
        *(int32_t *)(where + size - sizeof(val)) = htonl(val);
        /* place the int */
        *where |= 0x80;
        /* set that high–order bit */
    }
    return err;
}

unsigned int calc_checksum(Header *header_struct)
{
    /* all the numbers used are taken from the header specification*/
    unsigned int checksum = 0;
    int i = 0;
    for (i = 0; i < NAME_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->name[i];
    }
    for (i = 0; i < MODE_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->mode[i];
    }

    /* return 0 (invalid checksum because it is at least 8* spaces) to signal
     * an invalid header*/
    if (strict && header_struct->uid[0] == 0x80)
    {
        return 0;
    }
    else
    {
        for (i = 0; i < UID_SIZE; i++)
        {
            checksum += (unsigned char)header_struct->uid[i];
        }
    }
    for (i = 0; i < GID_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->gid[i];
    }
    for (i = 0; i < SIZE_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->size[i];
    }
    for (i = 0; i < MTIME_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->mtime[i];
    }
    for (i = 0; i < CHKSUM_SIZE; i++)
    {
        checksum += (unsigned char)' ';
    }
    for (i = 0; i < TYPEFLAG_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->typeflag[i];
    }
    for (i = 0; i < LINKNAME_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->linkname[i];
    }
    for (i = 0; i < MAGIC_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->magic[i];
    }
    for (i = 0; i < VERSION_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->version[i];
    }
    for (i = 0; i < UNAME_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->uname[i];
    }
    for (i = 0; i < GNAME_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->gname[i];
    }
    for (i = 0; i < DEVMAJOR_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->devmajor[i];
    }
    for (i = 0; i < DEVMINOR_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->devminor[i];
    }
    for (i = 0; i < PREFIX_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->prefix[i];
    }
    for (i = 0; i < PADDING_SIZE; i++)
    {
        checksum += (unsigned char)header_struct->padding[i];
    }
    return checksum;
}

int validate_header(Header *header_struct)
{
    unsigned int chcksum_calc = calc_checksum(header_struct);
    unsigned int chcksum_stored = strtol(header_struct->chksum,
                                         NULL, OCTALFLAG);

    /* used for unstrict checking*/
    char magic_buf_unstrict[MAGIC_SIZE];
    memcpy(magic_buf_unstrict, header_struct->magic, MAGIC_SIZE);

    /* set last char to nul termination to fit unstrict mode*/
    magic_buf_unstrict[MAGIC_SIZE - 1] = '\0';

    /* always check chksum and 5 characters of ustar*/
    if (chcksum_calc != chcksum_stored)
    {
        return 0;
    }
    else if (strcmp(magic_buf_unstrict, "ustar"))
    {
        return 0;
    }

    /* in strict mode check for nulltermination and version number*/
    if (strict)
    {
        if (strcmp(header_struct->magic, "ustar"))
        {
            fprintf(stderr, "Magic not valid.\n");
            return 0;
        }
        else if (header_struct->version[0] != '0' ||
                 header_struct->version[1] != '0')
        {
            fprintf(stderr, "Version not valid.\n");
            return 0;
        }
    }
    return 1;
}

Header *extract_header(char *header_data)
{
    /* extract header out of read data*/
    Header *header_struct = (Header *)calloc(sizeof(Header), sizeof(char));
    if (!header_struct)
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    /* because of our packed attribute we can just memcopy the header data*/
    memcpy(header_struct, header_data, BLOCK_SIZE);
    return header_struct;
}

int check_archive_end(int tar_filedes)
{
    /* check if end of archive is reached by checking if 2 blocks of BLOCK_SIZE
     * are '\0'*/
    off_t current_offset = lseek(tar_filedes, 0, SEEK_CUR);
    if (current_offset == -1)
    {
        perror("lseek");
        exit(EXIT_FAILURE);
    }
    char buffer[2 * BLOCK_SIZE];
    if (read(tar_filedes, buffer, 2 * BLOCK_SIZE) == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }
    int i;
    /* check if last two blocks are reached*/
    for (i = 0; i < 2 * BLOCK_SIZE; i++)
    {
        if (buffer[i] != '\0')
        {
            /* reset offset*/
            if (lseek(tar_filedes, current_offset, SEEK_SET) == -1)
            {
                perror("lseek");
                exit(EXIT_FAILURE);
            }
            return 0;
        }
    }
    return 1;
}

void build_name(Header *header_struct, char *full_name)
{
    /* numbers taken from specification +1 for \0 termination*/
    char prefix[PREFIX_SIZE + 1];
    char name[NAME_SIZE + 1];

    /* copy name*/
    memcpy(name, header_struct->name, NAME_SIZE);
    /* don't forget to NULL terminate*/
    name[NAME_SIZE] = '\0';
    /* print out name of file*/
    /* include prefix if applicable*/
    if (header_struct->prefix[0] != '\0')
    {
        /* copy prefix*/
        memcpy(prefix, header_struct->prefix, PREFIX_SIZE);
        /* NUL Terminate for printf to work*/
        prefix[PREFIX_SIZE] = '\0';
        sprintf(full_name, "%s/%s", prefix, name);
    }
    else
    {
        /* handle if directory barely fits in name buffer*/
        if (header_struct->typeflag[0] == '5' && strlen(name) == NAME_SIZE)
        {
            sprintf(full_name, "%s/", name);
        }
        else
        {
            sprintf(full_name, "%s", name);
        }
    }
}
