#include "create_util.h"
#include "list_util.h"
#include "extract_util.h"

int main(int argc, char *argv[])
{
    int tar_filedes;
    /* index var*/
    int i;

    /* command line options are defined in uitility.h to be used in every
     * function*/
    /* argument for checking*/
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
            if (argc < 4)
            {
                fprintf(stderr,
                        "Please provide path to archive.\n");
                exit(EXIT_FAILURE);
            }

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
    /* I allow the create/listing/extracting arguments to be present
     * simultaneously because i prioritize creating over listing over
     * extracting*/
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
        /* write two blocks of \0 to mark end of archive*/
        char buffer[2 * BLOCK_SIZE];
        /* set buffer to 0*/
        memset(buffer, '\0', 2 * BLOCK_SIZE);
        /* write data to file*/
        if (write(tar_filedes, buffer, 2 * BLOCK_SIZE) == -1)
        {
            perror("writing");
            exit(EXIT_FAILURE);
        }
    }
    else if (listing)
    {
        int num_paths = 1;
        if (argc > 3)
        {
            /* create path shopping list :*/
            /* number of arguments - first three arguments*/
            num_paths = argc - 3;
            char *shopping_list[num_paths];

            for (i = 3; i < argc; i++)
            {
                if (strlen(argv[i]) > PATH_LENGTH)
                {
                    perror("path too long");
                    continue;
                }
                else
                {
                    shopping_list[i - 3] = argv[i];
                }
            }
            /* now go window shopping*/
            list_tar(shopping_list, tar_filedes, num_paths);
        }
        else
        {
            /* empty string always compares positive in strstr*/
            char *shopping_list[num_paths];
            shopping_list[0] = "";
            /* now go window shopping*/
            list_tar(shopping_list, tar_filedes, num_paths);
        }
    }

    else if (extract)
    {
        int num_paths = 1;
        if (argc > 3)
        {
            /* create path shopping list :*/
            /* number of arguments - first three arguments*/
            num_paths = argc - 3;
            char *shopping_list[num_paths];
            for (i = 3; i < argc; i++)
            {
                if (strlen(argv[i]) > PATH_LENGTH)
                {
                    perror("path too long");
                    continue;
                }
                else
                {
                    shopping_list[i - 3] = argv[i];
                }
            }
            /*now go actually shopping*/
            extract_archive(shopping_list, tar_filedes, num_paths);
        }
        else
        {
            /* empty string always compares positive in strstr*/
            char *shopping_list[num_paths];
            shopping_list[0] = "";

            /*now go actually shopping*/
            extract_archive(shopping_list, tar_filedes, num_paths);
        }
    }

    close(tar_filedes);
    return 0;
}