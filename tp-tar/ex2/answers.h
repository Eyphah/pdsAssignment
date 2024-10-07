#pragma once

#include <limits.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_FILES    128

struct header {
   char name[128];      // chemin relative du fichier
   int size;            // taille en octets
};

// ATTENTION : au lieu de printf, utilisez les 2 fonctions suivantes
// pour afficher vos résultats, sinon les test ne passeront pas.
void output_str(char *string);
void output_bytes(char *array, int n);


/* Exercice 1, question 1 */
int list_archive(const char *archive_name);

/* Exercice 1, question 2 */
struct loffsets {
    int n_files;             // nombre de fichiers dans l'archive
    off_t offset[MAX_FILES]; // positions des entêtes
};

int generate_offsets(const char *archive_name, struct loffsets *lo);

/* Exercice 1, question 3 */
int print_file(const char *archive_name, struct loffsets *lo, int n);

/* Exercice 1, question 4 */
int add_file(const char *archive_name, const char *filename);
