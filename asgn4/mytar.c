#include "utility.h"

int main(int argc, char *argv[])
{
    int tar_filedes;

    /* index var*/
    int i;

    /* command line options are defined in uitility.h to be used in every
    function*/

    char argument;
    /* check for right usage*/
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s [ctxvS]f tarfile [ path [ ... ] ]",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    else
    {
        /* check for command line arguments and set values accordingly*/

        for (i = 0; (argument = argv[1][i]) != '\0'; i++)
        {
            switch (argument)
            {
            case 'c':
                create = 1;
                break;
            case 't':
                listing = 1;
                break;
            case 'x':
                extract = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'S':
                strict = 1;
                break;
            case 'f':
                file = 1;
                break;
            default:
                fprintf(stderr, "unrecognized option '%c'.\n", argument);
                exit(EXIT_FAILURE);
                break;
            }
        }

        /* exit if no 'f' argument was provided*/
        if (!file)
        {
            fprintf(stderr, "f Argument is missing.");
            exit(EXIT_FAILURE);
        }

        /* priority is creating if multiple arguments are given*/
        /* open tar file according to command line arguments*/
        if (create)
        {
            /* open for writing & truncating if in create mode*/
            tar_filedes = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC,
                               S_IRWXU | S_IRWXG);
            if (tar_filedes == -1)
            {
                perror(argv[2]);
                exit(EXIT_FAILURE);
            }
        }
        else if (extract || listing)
        {
            /* open for reading if in listing or extracting mode*/
            tar_filedes = open(argv[2], O_RDONLY);
            if (tar_filedes == -1)
            {
                perror(argv[2]);
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            /* exit if none of the required arguments is provided*/
            fprintf(stderr, "one of c', 't' or 'x' argument is missing.");
            exit(EXIT_FAILURE);
        }
    }

    if (create)
    {
        /* 4 is the number of arguments before the pathnames start -> index 3*/
        for (i = 3; i < argc; i++)
        {
            if (strlen(argv[i]) > PATH_LENGTH)
            {
                perror("path too long");
                continue;
            }
            else
            {
                archive_file(argv[i], tar_filedes);
            }
        }
        /* write two blocks of 0 to mark end of archive*/
        char buffer[2 * BLOCK_SIZE];
        /* set buffer to 0*/
        for (i = 0; i < 2 * BLOCK_SIZE; i++)
        {
            buffer[i] = '\0';
        }
        if (write(tar_filedes, buffer, 2 * BLOCK_SIZE) == -1)
        {
            perror("writing");
            exit(EXIT_FAILURE);
        }
    }
    else if (listing)
    {
        if (argc > 3)
        {
            for (i = 3; i < argc; i++)
            {
                if (strlen(argv[i]) > PATH_LENGTH)
                {
                    perror("path too long");
                    continue;
                }
                else
                {
                    list_tar(argv[i], tar_filedes);
                }
            }
        }
        else
        {
            list_tar("", tar_filedes);
        }
    }

    close(tar_filedes);
    return 0;
}