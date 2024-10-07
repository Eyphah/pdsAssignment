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

int generate_offsets(const char *archive_name, struct loffsets *lo) //en gros on lit les entetes et on donne leur position en leur retirant leur taille puisqu'on les lit jusu'au bout avant de retourner la valeur
{
    FILE *f = fopen(archive_name, "rb"); //rb pr lire fichiers bin
    if(f==NULL){
        return -1;
    }

    lo->n_files=0; //initialiser à chaque appel sinon on peut commencer à compter à partir d'une valeur indéfinie
    struct header h;
    while(fread(&h, sizeof(struct header), 1,f)>0){ //on lit 1 par 1 les bytes d'un entête
        if(lo->n_files >= MAX_FILES){ //check si trop de fichiers
            fclose(f);
            return -1;
        }
        lo->offset[lo->n_files ] = ftell(f) - sizeof(struct header); //position de l'entête (enfait on lit jusqu'à la fin de l'entête dc pour avoir le début on lui soustrait sa taille)
        lo->n_files++;
        fseek(f,h.size,SEEK_CUR); //on utilise header pour pouvoir keep track de la taille des fichiers
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

    FILE *f = fopen(archive_name, "rb"); //réouvre car generate_offsets close le fichier
    if(f==NULL){
        return -1;
    }
    int offset = lo->offset[n];
    int size = (n+1<lo->n_files)?(lo->offset[n+1]-offset):(lo->offset[n]+sizeof(struct header));
    
    fseek(f, offset, SEEK_SET); //on se positionne après l'entête du fichier
    
    char *fileContent = (char *)malloc(size*sizeof(char));
    if (fileContent == NULL){
        fclose(f);
        return -1; //echec d'allocation donc on termine
    }
    fread(fileContent, sizeof(char),size,f); //on lit l'entièreté du fichier
    output_bytes(fileContent,size);
    free(fileContent);
    fclose(f);
    return 0;
}

int add_file(const char *archive_name, const char *filename)
{
    // TODO

    return -123;
}
