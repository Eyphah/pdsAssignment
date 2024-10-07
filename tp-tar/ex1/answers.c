#include "answers.h"

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define SIZE 2000

extern void output_str(char *string);
extern int TEST_FILENO;

int affiche_file(char *filename)
{
    int file = open(filename, O_RDONLY);
    if (file == -1){
        return -1;
    }

    char * contentBuf = malloc(sizeof(char)*SIZE);
    assert(contentBuf != NULL);
    int n;
    while((n=read(file,contentBuf,sizeof(char)*SIZE))>0){
        write(TEST_FILENO, contentBuf, n);
    }
    free(contentBuf);
    close(file);
    return 0;
}


void ecris_tableau(int nums[], int n, char *filename)
{
    // TODO
}

void ecris_tableau_ascii(int nums[], int n, char *filename)
{
    // TODO
}
