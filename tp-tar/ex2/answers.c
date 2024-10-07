#include "answers.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define PATH_MAX 2000

int list_archive(const char *archive_name)
{
    FILE *f = fopen(archive_name, "r");
    if (f == NULL) //on teste si un pointer est nul et pas égal à -1
    {
        return -1;
    }
    struct header h;
    while (fread(&h, sizeof(char),sizeof(h),f) > 0)
    {
        output_str(h.name);
        fseek(f, h.size, SEEK_CUR);
    }
    fclose(f);
    return 0;
}

int generate_offsets(const char *archive_name, struct loffsets *lo) //we count the amount of files within the archive and for each of them put their header offset (where their header is) in an array whos index matches the file number
{
    FILE *f = fopen(archive_name, "rb"); //rb pr lire fichiers bin / rb to read binary files
    if(f==NULL){
        return -1;
    }

    lo->n_files=0; //initialisation to avoid the values stacking if multiple function calls
    struct header h;
    while(fread(&h, sizeof(struct header), 1,f)>0){ //read the header and store it in a header struct
        if(lo->n_files >= MAX_FILES){ //check if too many files
            fclose(f);
            return -1;
        }
        lo->offset[lo->n_files ] = ftell(f) - sizeof(struct header); //pos of the header ( we read until the end of it so we susbtract its size for its first position)
        lo->n_files++;
        fseek(f,h.size,SEEK_CUR); 
    }
    fclose(f);
    return 0;
}

int print_file(const char *archive_name, struct loffsets *lo, int n)
{
    if(generate_offsets(archive_name,lo)!=0){
        return -1;
    }

    if(n<0 || n>=lo->n_files){
        return -2;
    }

    FILE *f = fopen(archive_name, "rb"); //reopen bc generate_offsets closes the file
    if(f==NULL){
        return -1;
    }
    int offset = lo->offset[n];
    int size = (n+1<lo->n_files)?(lo->offset[n+1]-offset):(lo->offset[n]+sizeof(struct header));
    
    fseek(f, offset, SEEK_SET); //putting cursor at beginning of file
    
    char *fileContent = (char *)malloc(size*sizeof(char));
    if (fileContent == NULL){
        fclose(f);
        return -1; //memory allocation failed so stopping process
    }
    fread(fileContent, sizeof(char),size,f); //read file number n
    output_bytes(fileContent,size); //write fileContent
    free(fileContent);
    fclose(f);
    return 0;
}

int add_file(const char *archive_name, const char *filename)
{
    // TODO

    return -123;
}
