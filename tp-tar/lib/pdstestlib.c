#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <getopt.h>


#include "pdstestlib.h"

/*
   Copyright 2023, Pierre Graux (pierre.graux@univ-lille.fr) and
   Giuseppe Lipari (giuseppe.lipari@univ-lille.fr). 

   This file is part of pds_testlib, the PDS Test Library. This is a
   library for testing exercices and exams for the cours "PDS" in
   Licence 3 Info/Miage at the University of Lille.

   pds_testlib is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see
   <https://www.gnu.org/licenses/>.
*/

/**
   Adds n points for the test t
 */
#define ADD_POINT(file, t, n) output_note(file, (t), (n))


static int opt_verbose = 0;

/* ---------- UTILS ----------- */
void trim_string(char *str)
{
    int start = 0, end = strlen(str) - 1;
    while (isspace((unsigned char)str[start]))
        start++;

    while ((end >= start) && isspace((unsigned char)str[end]))
        end--;

    str[end + 1] = '\0';
    memmove(str, str + start, end - start + 1);
}

void remove_double_slash(char *string)
{
    char *src;
    char *dst;

    if (strlen(string) == 0) return;
    
    for (dst = src = (string+1); *src != 0; src++) {
        *dst = *src;
        if (!(*src == '/' && *(src-1) == '/')) dst++; 
    }
    *dst = 0;        
}

/* ---------- HELPER FUNCTIONS ---------- */

void test_info(const char* info)
{
    printf("\t\033[1;31m%s\033[0m\n", info);
}

void test_passed(const char* test_name, const char* descr)
{
    printf("\033[;32mTest-case '%s': PASSED\033[0m\n\t%s\n", test_name, descr);
}

void test_failed(const char* test_name, const char* descr)
{
    printf("\033[1;31mTest-case '%s': FAILED\033[0m\n\t%s\n", test_name, descr);
}


void output_note(const char* filename, const char *exname, int n)
{
    FILE* f = NULL;
    // TODO : retry if already open
    f = fopen(filename, "a");
    assert(f != NULL);
    
    fprintf(f, "%s=%i, ", exname, n);
    fclose(f);
}

/* --------- NEW INTERFACE ------------ */

#define MAX_NUM_TESTS 128

static int tc_last = 0;
struct test_case tc_array[MAX_NUM_TESTS];
struct test_case *tc_current = NULL;

int test_case_find(char *key)
{
    if (key == NULL) {
        if (tc_last > 0) return 0;
        else return -1;
    }
    
    for (int i=0; i<tc_last; i++) {
        if (strncmp(tc_array[i].name, key, strlen(key)) == 0) 
            return i;
    }
    return -1; // NOT FOUND
}

int test_case_next(char *key, int l)
{
    if (key == NULL) {
        if (l < (tc_last - 1)) return l+1;
        else return -1;
    }
    
    for (int i=l+1; i<tc_last; i++) {
        if (strncmp(tc_array[i].name, key, strlen(key)) == 0) 
            return i;
    }
    return -1; // NOT FOUND
}


int test_case_register(struct test_case tc)
{
    if (test_case_find(tc.name) != -1) {
        fprintf(stderr, "Warning, test_case_register() failed: test %s exists already\n",
                tc.name);
        return -1;
    }
    
    tc_array[tc_last++] = tc;
    
    return tc_last-1;
}

void test_case_start(struct test_case *tc)
{
    assert(tc_current == NULL);
    tc_current = tc;
    printf("Running test-case '%s' ...\n", tc_current->name);
    if (opt_verbose) {
        printf("Description: %s\n", tc_current->description);
    }
    TEST_FILENO = open(TEST_OUTPUT_FILENAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    assert(TEST_FILENO > 0);
}

void test_case_end(bool passed)
{
    assert(tc_current != NULL);
    close(TEST_FILENO);
    if (passed) {
        test_passed(tc_current->name, "");
        ADD_POINT(tc_current->filename, tc_current->name, tc_current->points);
    }
    else {
        test_failed(tc_current->name, tc_current->failmsg);
        ADD_POINT(tc_current->filename, tc_current->name, 0);
    }
    tc_current = NULL; // signals that the test is over
}


int test_case_run(int test_num)
{
    sigset_t mask;
    sigset_t orig_mask;
    struct timespec timeout;

    sigemptyset (&mask);
    sigaddset (&mask, SIGCHLD);

    if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
        perror ("sigprocmask");
        return 1;
    }

    assert(test_num < tc_last && test_num >= 0);

    test_case_start(&tc_array[test_num]);
    pid_t pid = fork();
    if (pid == 0) {
        bool result = tc_array[test_num].fun();
        test_case_end(result);
        
        if (result) exit(EXIT_SUCCESS); 
        else exit(EXIT_FAILURE);
    }
    
    timeout.tv_sec = tc_array[test_num].timeout;
    timeout.tv_nsec = 0;
    
    do {
        if (sigtimedwait(&mask, NULL, &timeout) < 0) {
            if (errno == EINTR) {
                /* Interrupted by a signal other than SIGCHLD. */
                continue;
            }
            else if (errno == EAGAIN) {
                //tc_array[test_num].failmsg = malloc(1024);
                sprintf(tc_array[test_num].failmsg,
                        "Timeout. La fonction de test a pris plus de %d seconds.\n"
                        "\tPeut-être une boucle infinie ?", tc_array[test_num].timeout);
                test_case_end(false);
                kill(pid, SIGKILL);
            }
            else {
                perror("sigtimedwait");
                sigprocmask(SIG_SETMASK, &orig_mask, NULL);
                return 1;
            }
        }
        break;
    } while (1);

    tc_current = NULL;
    int status;
    
    if (waitpid(pid, &status, 0) < 0) {
        perror ("waitpid");
        sigprocmask(SIG_SETMASK, &orig_mask, NULL);
        return TEST_FAILURE;
    }
    sigprocmask(SIG_SETMASK, &orig_mask, NULL);

    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == EXIT_SUCCESS) return TEST_SUCCESS;
        else return TEST_FAILURE;
    }
    else return TEST_FAILURE;
}

int test_main(int argc, char *argv[])
{
    int opt;
    int opt_list = 0;
    int stop_on_fail = 0;
    
    char *pattern = NULL;
        
    while ((opt = getopt(argc, argv, "vls")) != -1) {
        switch(opt) {
        case 'v' :
            opt_verbose = 1;
            break;
        case 'l' :
            opt_list = 1;
            break;
        case 's' :
            stop_on_fail = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-v] [-l] [-s] [pattern]\n",
                   argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (optind < argc) 
        pattern = argv[optind];

    /* Run all selected tests */
    
    int f = test_case_find(pattern);
    int totalpoints = 0;
    while (f >= 0) {
        if (opt_list) {
            printf("Test name   : %s\n", tc_array[f].name);
            if (opt_verbose) {
                printf("Description : %s\n", tc_array[f].description);
                printf("Points      : %d\n", tc_array[f].points);
                printf("----------------\n");
            }
            totalpoints += tc_array[f].points;
        }
        else {
            bool r = test_case_run(f);
            if (r) totalpoints += tc_array[f].points;
            if (stop_on_fail && !r) break; // stop on first failure
        }
        
        f = test_case_next(pattern, f);
    }
    printf("Total points : %d\n", totalpoints);

    
    return EXIT_SUCCESS;
}            


/* --------- BASIC TEST INFRASTRUCTURE --------- */

/* static struct pds_test_case { */
/*     const char *filename; */
/*     const char *test_case_name; */
/*     int points; */
/* } curr_test; */
/* */

int TEST_FILENO = -1; 

/* void start_testcase(const char *f, const char *t, int p) */
/* { */
/*     assert(curr_test.filename == NULL); */
/*     curr_test.filename = f; */
/*     curr_test.test_case_name = t; */
/*     curr_test.points = p; */
    
/*     printf("Test-case '%s' ...\n", curr_test.test_case_name); */
/* } */

/* void end_test(bool passed) */
/* { */
/*     assert(curr_test.filename != NULL); */
/*     close(TEST_FILENO); */
/*     if (passed) { */
/*         test_passed(curr_test.test_case_name, ""); */
/*         ADD_POINT(curr_test.filename, curr_test.test_case_name, curr_test.points); */
/*     } */
/*     else { */
/*         test_failed(curr_test.test_case_name, ""); */
/*         ADD_POINT(curr_test.filename, curr_test.test_case_name, 0); */
/*     } */
/*     curr_test.filename = NULL; // signals that the test is over.  */
/* } */

int exec_and_parse_int(const char* cmd, int* error_occured)
{
    FILE* f;

    f = popen(cmd, "r");
    if(f != NULL) {
        int res;
        if(fscanf(f, "%i", &res) != EOF) {
            if(error_occured != NULL)
                *error_occured = 0;
            return res;
        }
    }

    if(error_occured != NULL)
        *error_occured = 1;
    return 0;
}


/* int fork_test_and_wait(const char *filename, const char *testname, */
/*                        int points, test_fun fun, int tout) */
/* { */
/*     sigset_t mask; */
/*     sigset_t orig_mask; */
/*     struct timespec timeout; */

/*     sigemptyset (&mask); */
/*     sigaddset (&mask, SIGCHLD); */

/*     if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) { */
/*         perror ("sigprocmask"); */
/*         return 1; */
/*     } */
    
/*     pid_t pid = fork(); */
/*     if (pid == 0) { */
/*         fun(filename, testname, points); */
/*         exit(0); */
/*     } */
    
/*     timeout.tv_sec = tout; */
/*     timeout.tv_nsec = 0; */
    
/*     do { */
/*         if (sigtimedwait(&mask, NULL, &timeout) < 0) { */
/*             if (errno == EINTR) { */
/*                 /\* Interrupted by a signal other than SIGCHLD. *\/ */
/*                 continue; */
/*             } */
/*             else if (errno == EAGAIN) { */
/*                 test_failed(testname, "--");                  */
/*                 print_err ("%s", "Timeout, killing child\n"); */
/*                 kill(pid, SIGKILL); */
/*             } */
/*             else { */
/*                 perror("sigtimedwait"); */
/*                 return 1; */
/*             } */
/*         } */
         
/*         break; */
/*     } while (1); */
     
/*     if (waitpid(pid, NULL, 0) < 0) { */
/*         perror ("waitpid"); */
/*         return 1; */
/*     } */
/*     return 0; */
/* }  */

/* ----------- OUTPUT STRINGS ------------- */

static struct output_str_s {
    char **expected_output;   // List of strings expected as output
    int expected_number;      // How many strings to expect
    bool *expected_found;     // How many have been found
    bool output_failed;       // Final result
} curr_output = { NULL, 0, NULL, false};


int output_has_failed()
{
    for (int i=0; i<curr_output.expected_number; i++) {
        if (curr_output.expected_found[i] == false) {
            print_err("%s not found", curr_output.expected_output[i]);
            curr_output.output_failed = true;
        }
    }
    
    return curr_output.output_failed;
}

void clear_expected_output()
{
    for (int i=0; i<curr_output.expected_number; i++)
        curr_output.expected_found[i] = false;
    curr_output.output_failed = false;
}

void init_expected_output(char **exp, int n)
{
    curr_output.expected_output = exp;
    curr_output.expected_number = n;
    curr_output.expected_found = (bool *)malloc(sizeof(int) * n);

    clear_expected_output();
}

void destroy_expected_output()
{
    free(curr_output.expected_found);
}

void output_str(char* string)
{
    bool is_expected = false;

    remove_double_slash(string);
    
    for(int i=0; i<curr_output.expected_number ; i++) {
        if(!strcmp(string, curr_output.expected_output[i])) {
            is_expected = true;
            if(curr_output.expected_found[i] == true) {
                print_err("%s printed several times", curr_output.expected_output[i]);
                curr_output.output_failed = true;
            } else
                curr_output.expected_found[i] = true;
        }
    }
    if(!is_expected) {
        printf("\033[1;31m\t%s should not be printed\033[0m\n", string);
        curr_output.output_failed = true;
    }
}


/*
  It can only be used for arrays that do not contain any 0. 
 */
void output_bytes(char* array, int n)
{
    bool is_expected = false;
    for(int i=0; i<curr_output.expected_number ; i++) {
        int len = strlen(curr_output.expected_output[i]);
        if ((n == len) && (memcmp(array,curr_output.expected_output[i], n) == 0)) {
            is_expected = true;
            if(curr_output.expected_found[i] == true) {
                print_err("%s printed several times", curr_output.expected_output[i]);
                curr_output.output_failed = true;
            }
            else curr_output.expected_found[i] = true;
        }
    }
    if(!is_expected) {
        print_err("%s", "array should not be printed");
        curr_output.output_failed = true;
    }
}


bool check_test_file()
{
    close(TEST_FILENO);
    int fd = open(TEST_OUTPUT_FILENAME, O_RDONLY);
    if (fd<0) {
        printf("\033[1;31m\tCould not find file \"%s\"\033[0m\n", TEST_OUTPUT_FILENAME);
        return false;
    }

    for (int i=0; i< curr_output.expected_number; i++) {
        int nexp = strlen(curr_output.expected_output[i]); 
        char buff[nexp+1];
        int nr = read(fd, buff, nexp);
        buff[nr] = 0;
        if (strcmp(curr_output.expected_output[i], buff)) {
            printf("\033[1;31m\toutput \"%s\" does not match\033[0m\n", buff);
            printf("\033[1;31m\texpected: \"%s\"\033[0m\n", curr_output.expected_output[i]);
            curr_output.output_failed = true;
            close(fd);
            return false;
        }
    }
    return true;
}



