#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/pdstestlib.h"
#include "../ex1/answers.h"

bool test_ex1q1a()
{
    int r = affiche_file("nonexistent");

    return r == -1;
}

bool test_ex1q1b()
{
    char *exp_out[] = {"Hello PDS !\n", "Ton premier exercice est correct"};
    init_expected_output(exp_out, sizeof(exp_out)/sizeof(char *));

    int r = affiche_file("toto.txt");
    if (r == -1) return false;
    else return check_test_file();
}


#define FILENAME "results.txt"

extern struct test_case tc_array[];

int main(int argc, char*argv[])
{
    struct test_case tc1 = {
        .filename = FILENAME,
        .name = "Ex1-Q1-A",
        .description = "Teste affiche_file",
        .failmsg = "La fonction doit retourner -1 sur un fichier qui n'existe pas",
        .points = 1,
        .fun = test_ex1q1a,
        .timeout = 1  
    };
    test_case_register(tc1);

    struct test_case tc2 = {
        .filename = FILENAME,
        .name = "Ex1-Q1-B",
        .description = "Teste affiche_file",
        .failmsg = "La fonction doit retourner -1 sur un fichier qui n'existe pas",
        .points = 1,
        .fun = test_ex1q1b,
        .timeout = 1  
    };
    test_case_register(tc2);
    
    return test_main(argc, argv);
}
