#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../lib/pdstestlib.h"
#include "../ex2/answers.h"


bool test_ex2q1a()
{
    char *exp_out[] = {"a.txt", "b.txt", "c.txt", "d.txt"}; 
    init_expected_output(exp_out, sizeof(exp_out)/sizeof(char *));

    list_archive("ar2");
    
    return !output_has_failed(); // check if output_str() has failed 
}

bool test_ex2q1b()
{
    char *exp_out[] = {}; 
    init_expected_output(exp_out, 0);

    int r1 = list_archive("doesnotexist.pdsar");
    printf("r1 = %d\n", r1);
    if (r1 != -1) 
        return false;
    else 
        return !output_has_failed(); // check if output_str() has failed
}

bool test_ex2q2()
{
    struct loffsets exp = { .n_files = 4 };
    exp.offset[0] = 0;
    exp.offset[1] = 139;
    exp.offset[2] = 274;
    exp.offset[3] = 416;
   
    struct loffsets lo;
    generate_offsets("ar2", &lo);

    bool result = true;
    for (int i=0; i<4; i++) {
        if (lo.offset[i] != exp.offset[i]) {
            print_err("lo.offset[%d] attendu : %ld,  obtenu %ld", i, exp.offset[i], lo.offset[i]);
            result = false;
        }
    }
    return result;
}

bool test_ex2q3()
{
    char *exp_out[] = {"abcdefg", "hjk", "0123456789", "%^*0./"};
    init_expected_output(exp_out, 4);

    struct loffsets lo;
    generate_offsets("ar2", &lo);
    print_file("ar2", &lo, 0);
    print_file("ar2", &lo, 1);
    print_file("ar2", &lo, 2);
    print_file("ar2", &lo, 3);
    
    return !output_has_failed();
}

bool test_ex2q4()
{
    add_file("ar3", "data.txt");
    
    FILE *f = fopen("ar3", "r");
    fseek(f, -12, SEEK_END);
    char buffer[100];
    fgets(buffer,13, f);
    int result = !strcmp(buffer, "Q4 succeded\n");

    struct stat s1;
    stat("ar3", &s1);
    struct stat s2;
    stat("arq4", &s2);

    TEST_EQ(s1.st_size, s2.st_size, result);
        
    return result;
}

#define FILENAME "results.txt"

extern struct test_case tc_array[];

int main(int argc, char *argv[])
{
    struct test_case tc1 = {
        .filename = FILENAME,
        .name = "Ex2-Q1-A",
        .description = "Verifie que le fichier ar2 contient les bons fichier. L'ordre d'affichage n'est pas important.",
        .failmsg = "La fonction list_archive() n'affiche pas les bons fichiers. Avez-vous utilisé output_str() pour afficher ?",
        .points = 1,
        .fun = test_ex2q1a,
        .timeout = 1
  
    };
    test_case_register(tc1);


    struct test_case tc2 = {
        .filename = FILENAME,
        .name = "Ex2-Q1-B",
        .description = "La fonction list_archive() doit retourner -1 pour les fichiers qui n'existent pas",
        .failmsg = "La fonction list_archive() ne retourne pas -1 pour le fichier doesnotexist.pdsar",
        .points = 1,
        .fun = test_ex2q1b,
        .timeout = 1
    };
    test_case_register(tc2);


    struct test_case tc3 = {
        .filename = FILENAME,
        .name = "Ex2-Q2",
        .description = "Calcul des offsets",
        .failmsg = "La fonction generate_offsets(\"ar\", &lo) ne calcule pas correctment les offsets",
        .points = 1,
        .fun = test_ex2q2,
        .timeout = 1        
    };
    test_case_register(tc3);


    struct test_case tc4 = {
        .filename = FILENAME,
        .name = "Ex2-Q3",
        .description = "affichage des fichiers",
        .failmsg = "La fonction print_file(\"ar2\", &lo, i) n'affiche pas correctement les fichiers",
        .points = 1,
        .fun = test_ex2q3,
        .timeout = 1        
    };
    test_case_register(tc4);

    struct test_case tc5 = {
        .filename = FILENAME,
        .name = "Ex2-Q4",
        .description = "Teste la fonction add_file()",
        .failmsg = "Le fichier n'est pas ajouté correctment",
        .points = 1,
        .fun = test_ex2q4,
        .timeout = 1        
    };
    test_case_register(tc5);

    return test_main(argc, argv);    
}
