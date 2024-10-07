#include "answers.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void output_str(char *string)
{
    printf("%s\n", string);
}

void output_bytes(char *bytes, int n)
{
    while (n-- > 0)
        printf("%c", *(bytes++));
}


int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s [options] <archive>\n", argv[0]);
        printf("Options are: \n");
        printf("   -l : list all files in the archive\n");
        printf("   -a file : add the file to the archive\n");
        printf("   -o n : outputs the n-th file in the archive\n");
        exit(EXIT_FAILURE);
    }

    int opt; int add = 0; int list=0; int out=0;
    char *file_to_add = NULL;
    int n = 0;

    while ((opt = getopt(argc, argv, "la:o:")) != -1) {
        switch(opt) {
        case 'l' : list = 1;
            break;
        case 'a':
            add = 1;
            file_to_add = optarg;
            break;
        case 'o':
            out = 1;
            n = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Unknown option\n");
            exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }

    struct loffsets lo = { .n_files = 0 };

    int r = generate_offsets(argv[optind], &lo);

    if (r < 0) {
        printf("Archive file does not exist\n");
        exit(EXIT_FAILURE);
    }

    for (int i=0; i<lo.n_files; i++) {
        printf("offset[%d] = %ld\n", i, lo.offset[i]);
    }
    
    if (list)  {
        printf("List of files in the archive %s:\n", argv[optind]);
        int nf = list_archive(argv[optind]);
        printf("The archive contains %d files\n", nf);
    }
    if (add)   {
        printf("Adding file %s to archive %s\n", file_to_add, argv[optind]);
        add_file(argv[optind], file_to_add);
    }
    if (out)   {
        printf("Printing the %d file in the archive %s\n", n, argv[optind]);
        int r = print_file(argv[optind], &lo, n);
        printf("Result : %d\n", r);
    }
    return 0;
}
