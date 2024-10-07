#pragma once

#include <stdio.h>
#include <stdbool.h>

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

#define MAXCHAR 512
#define MAXDESCR 1024

#define TEST_SUCCESS true
#define TEST_FAILURE false

#define TEST_OUTPUT_FILENAME "__TEST_FILE__"
extern int TEST_FILENO;


/**
   The testing function. It takes no parameter, it will return true
   if the test if succesful, and false if it fails.
 */
typedef bool (*test_fun2)();

/**
   A test case.

   A test case is identified by a filename where the results will be
   stored (in append mode), a test name, and how many points will be
   given if the test is passed. If the test fails, 0 points will be
   given.

   The test may has a description (to be shown when in verbose mode)
   and a message to be shown in case of failure to inform the user of
   what when wrong and maybe give some hint on how to solve the
   problem.

   The file is a comma-separated list of pairs "test_case_name=points".
 */ 
struct test_case {
    char filename[MAXCHAR];
    char name[MAXCHAR];
    char description[MAXDESCR];
    char failmsg[MAXDESCR];
    int points;
    test_fun2 fun;
    int timeout;
};

int test_case_register(struct test_case tc);

int test_case_run(int test_num);

int test_main(int argc, char *argv[]);

    
/* ----------- END OF NEW INTERFACE -------------- */
    
/**
   Starts a new test case. 
 */
void start_testcase(const char *filename,
                    const char *test_case_name,
                    int points);


/**
   Completes a test case that was started with the previous function.
   The user is informed and the points are given depending on the
   value of the boolean parameter.   
 */
void end_test(bool passed);


/**
   Prototype of a test-case function:
   - takes as input the name of the file where the test results
     will be stored, the name of this test case, and the amount
     of points to give if the test case is passed.
   - returns 0 if everything is ok, 1 if some assertion has failed.

   Typically, this function starts by calling start_testcase() and ends by
   calling end_testcase().

   The function may not complete (unfortunately, some students' code
   could enter an infinite loop). This means that the corresponding
   end_test() will never be called in such cases. This is not a big
   problem, as the function will almost always be called in a separate
   process. The only inconvinient is that the result file will be
   incomplete. I do not see any special way to treat this case: you
   may try to install an exit handler with atexit() that invokes the
   end_test(false), to be called when the process terminates, but I am
   not sure it will be called in case of abort() or segmentation fault ...
 */
typedef int (*test_fun)(const char *filename, const char *name, int points);

/**
   TODO : To be removed from the interface.
 */
void test_failed(const char* test_name, const char* descr);


/**
   Executes the external command "cmd", waits for it to finish, and
   returns the obtained value.
 */
int exec_and_parse_int(const char* cmd, int* error_occured);

/**
   For a test in an external process, and waits for it. It is possible
   to specify a timeout (tout): if the process does not terminate
   before the timeout expires, it returns an error.

   The test function takes the test file and the test name as
   parameters.

   TODO: deprecated
 */
int fork_test_and_wait(const char *filemane, const char *testname, int points,
                       test_fun fun, int timeoutout);

/** outputs an error with highlights */
#define print_err(fmt, ...) printf("\t\033[1;31m" fmt "\033[0m\n", __VA_ARGS__);


/**
   Macro to compare two results.
   - func1: function or expression;
   - func2: function or expression;
   - passed: variable containing the result of the comparison:
       it is set to false if the two values are different, 
       it is left unchanged if two values are equal.

   Therefore, you can call this macro several times in a sequence to
   implement AND conditions, for example like this:

     int result = 1; // true
     TEST_EQ(fun1(), 2, result);
     TEST_EQ(fun2(), 0, result);

   is equivalent to

     result = (fun1() == 2 && fun2() == 0);

   The difference is that the macro version of the code above will
   output on the terminal a description of the problem in case of
   failure of the test.
*/
#define TEST_EQ(func1, func2, passed) do {                              \
        int obt = (func1);                                              \
        int exp = (func2);                                              \
        if (obt != exp) {                                               \
            print_err("\tError\n\tFile : %s, Line : %d, %s == %s", __FILE__, __LINE__, #func1, #func2); \
            print_err("\tObtained => %d", obt);                         \
            print_err("\tExpected => %d", exp);                         \
            passed = false;                                             \
        }                                                               \
    }  while (0)

/* ---------------- */
/* HELPER FUNCTIONS */
/* ---------------- */
void init_expected_output(char **exp, int n);

#define INIT_OUTPUT(x) init_expected_output(x, sizeof(x)/sizeof(char *))

void clear_expected_output();
void destroy_expected_output();
int output_has_failed();
// For the user / student 
void output_str(char* string);

/*
  Version to give to students :
void output_str(char *string)
{
    printf("%s\n", string);
}
*/


void output_bytes(char *bytes, int n);

/*
  Version to give to students

void output_bytes(char *bytes, int n)
{
    while (n-- > 0)
        printf("%c", *(bytes++));
}
*/

bool check_test_file();


/**
   Helper function to remove consecutive slashes from a pathname.

   It replaces the string in place.
*/
void remove_double_slash(char *string);

/**
   Helper function to trim spaces from the string. It replaces the
   string in place.
 */
void trim_string(char *str);



