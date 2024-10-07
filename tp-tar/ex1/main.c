#include <unistd.h>
#include <stdio.h>
#include "answers.h"

void output_str(char *string)
{
    printf("%s\n", string);
}

int TEST_FILENO = STDOUT_FILENO;

int main()
{
    int r1 = affiche_file("toto.txt");
    printf("Resultat r1 : %d\n", r1);

    int nums[] = {1, 2, 3, 4, 5, 6, 7, 42, 54, 99};
    
    ecris_tableau(nums, 10, "int.bin");
    printf("Premier fichier écrit\n");
    
    ecris_tableau_ascii(nums, 10, "int.txt");        
}
