#pragma once

#include "pdstestlib.h"
#include <pthread.h>

#define MAX_LOGS 1000
#define ERRMSG_LEN 1024

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
  The log entry.

  - tid : id of the thread  that called the tlog_log() function

  - tid_valid : 1 if the thread_t tid variable is a valid thread
          id or not. Unfortunately, thread_t is an opaque type
          and the POSIX standard does not define a null value for
          this type of variable.

  - id : a character. Typically used to identify the thread type
        (e.g. 'P' as producer, 'C' as consumer) ; '*' matches "any"
        character in searches and comparisons.

  - n1 : a non-negative integer. Typically it is used as the index of
         the thread. A negative number matches "any" positive number
         in searches and comparisons.

  - n2 : a non-negative number. Typically, it indicates the
         operation. A negative number matches "any" positive number in
         searches and comparisons.
*/
typedef struct tlog_entry {
    int tid_valid;
    pthread_t tid;
    char id;
    int n1;
    int n2;
} tlog_entry;

/**
  A convenience function to create a structure tlog_entry.
  - a cannot be '*';
  - b and c cannot be negative.
 */
tlog_entry tlog_s(char a, int b, int c);

/**
   Logs an entry. The threads will call this during their
   execution. It uses atomic operations to protect the log array. The
   thread id is obtained through a call to pthread_self()
*/
void tlog_log(char id, int n1, int n2);

/**
   Clears the logs.
*/
void tlog_clear();

/**
   Prints all entries so far on stdout. 
 */
void tlog_print();

/**
   Returns the last error message. This is supposed to be called after
   the threads have completed execution. If you call it before,
   results are unpredictable.
 */
char *tlog_last_error_msg();

/**
   Checks if entry e is present in the log.
 */
int tlog_exists(tlog_entry e);

/**
    Check precedence constraints between two entries.

    Returns 1 if :
    - neither (b, b_n1, b_n2) nor (a, a_n1, a_n2) can
      be found in the logs.
    - if (a, a_n1, a_n2) exists and (b, b_n1, b_n2) comes after;
    Return 0 otherwise.

    Example :

       tlog_check_before(tlog_s('A', 0, 1) , tlog_s('B', 0, 1));

    returns 1 if the operation ('A', 0, 1) has been done
    before operation ('B', 0, 1).

    Wildcards are possible : '*' for the first field, a negative integer
    for the other two parameters.

    Example:

       tlog_check_before(tlog_s('A', -1, -1) , tlog_s('B', -1, -1));

    checks that all 'A's appear before all 'B's (typically, to check a barrier).
*/
int tlog_check_before(struct tlog_entry before, struct tlog_entry after);

/**
    Checks that all elements (id, n, n2) are present in any order,
    with n in [n1_min....n1_max-1]

    NO WILDCARDS!
*/
int tlog_check_all_n1(char id, int n1_min, int n1_max, int n2);

/**
    Checks that all elements (id, n1, n) are present in any order,
    with n2 in [n2_min....n2_max-1]

    NO WILDCARDS!
*/
int tlog_check_all_n2(char id, int n1, int n2_min, int n2_max);

/**
    Checks that all elements (id, n1, n) are present in incrementing order,
    with n in [n2_min....n2_max-1]

    NO WILDCARDS!
*/
int tlog_check_seq_n2(char id, int n1, int n2_min, int n2_max);

/** Macro to assert a property */
#define TASSERT(prop, errmsg, result) do {                              \
        if (!(prop)) {                                                  \
            printf("\033[1;31m[FAIL] file %s, line: %d\033[0m\n", __FILE__, __LINE__); \
            printf("Error : %s\n", errmsg);                             \
            tlog_print();                                               \
            result = 0;                                                 \
        }                                                               \
        else {                                                          \
            result = result && 1;                                       \
        }                                                               \
    } while (0)


typedef struct _tid_list {
    pthread_t *array;
    int size; 
    int n;
} tid_list_t;

void tid_list_init(tid_list_t *list);
void tid_list_destroy(tid_list_t *list);
int tid_list_find(tid_list_t *list, pthread_t elem);
void tlog_get_tid_list(tid_list_t *list);
int tlog_check_thread_barrier(tid_list_t *list);

    
