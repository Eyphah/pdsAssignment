#include <assert.h>
#include <stdio.h>
#include "tlogger.h"
#include <stdlib.h>

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

static struct tlog_entry logs[MAX_LOGS];
static char errmsg[ERRMSG_LEN] = "";
/* index of the first free entry, and number of logged entries. To be
   accessed using atomic instructions */ 
static int log_index = 0;

tlog_entry tlog_s(char a, int b, int c)
{
    struct tlog_entry e = { .tid_valid = 0, .id = a, .n1 = b, .n2 = c};
    return e;
}

void tlog_log(char id, int n1, int n2)
{
    assert(id != '*' && n1 >= 0 && n2 >= 0);
    int index = __atomic_fetch_add(&log_index, 1, __ATOMIC_SEQ_CST);
    logs[index].tid_valid = 1;
    logs[index].tid = pthread_self();
    logs[index].id = id;
    logs[index].n1 = n1;
    logs[index].n2 = n2;
}

void tlog_clear()
{
    int val = 0;
    __atomic_store(&log_index, &val, __ATOMIC_SEQ_CST);
}

void tlog_print()
{
    int n = __atomic_load_n(&log_index, __ATOMIC_SEQ_CST);
    for (int i=0; i<n; i++) {
        //printf("%d %ld %c(%d,%d)\n", logs[i].tid_valid, logs[i].tid, logs[i].id, logs[i].n1, logs[i].n2);
        printf("%c(%d,%d)\n", logs[i].id, logs[i].n1, logs[i].n2);
    }
}

char *tlog_last_error_msg()
{
    return errmsg;
}

static int char_eq(char a, char b)
{
    if (a == '*' || b == '*') return 1;
    else return a == b;
}

static int int_eq(int a, int b)
{
    if (a < 0 || b < 0) return 1;
    else return a == b;
}

int tlog_cmp(struct tlog_entry *pa, struct tlog_entry *pb)
{
    if (char_eq(pa->id, pb->id) && int_eq(pa->n1, pb->n1) && int_eq(pa->n2, pb->n2))
        return 1;
    else return 0;
}

int tlog_exists(tlog_entry e)
{
    int n = __atomic_load_n(&log_index, __ATOMIC_SEQ_CST);
    for (int i=0; i<n; i++)
        if (tlog_cmp(&e, &logs[i])) return 1;

    return 0;
}

int tlog_check_before(struct tlog_entry before, struct tlog_entry after)
{
    int n = __atomic_load_n(&log_index, __ATOMIC_SEQ_CST);
    int i, j;
    int i_max = -1;
    int j_min = MAX_LOGS; 
    /* search for all "before" */
    for (i=0; i<n; i++) {
        if (tlog_cmp(&before, &logs[i])) i_max = i > i_max ? i : i_max;
    }
    
    /* search for all "after" */
    for (j=0; j<n; j++) {
        if (tlog_cmp(&after, &logs[j])) j_min = j < j_min ? j : j_min;
    }

    /* if any "before" comes after any "after", it's an error */ 
    if (i_max < j_min) return 1;
    else {
        sprintf(errmsg, "\033[1;31mError:\033[0m %c(%d,%d) after %c(%d,%d)\n",
                logs[j_min].id, logs[j_min].n1, logs[j_min].n2,
                logs[i_max].id, logs[i_max].n1, logs[i_max].n2);
        return 0;
    }
}

int tlog_check_all_n1(char id, int n1_min, int n1_max, int n2)
{
    assert(n1_min < n1_max);
    int c[n1_max - n1_min];
    for (int i=0; i<(n1_max-n1_min); i++) c[i] = 0;
    
    int n = __atomic_load_n(&log_index, __ATOMIC_SEQ_CST);
    for (int i=0; i<n; i++)
        for (int x = n1_min; x<n1_max; x++) 
            if ((logs[i].id == id) && (logs[i].n1 == x) && (logs[i].n2 == n2)) {
                c[x-n1_min] = 1;
                break;
            }
    // TODO CHECK that all c are 1;
    for (int x=0; x<(n1_max - n1_min); x++)
        if (c[x] == 0) return 0;
    return 1;
}

int tlog_check_all_n2(char id, int n1, int n2_min, int n2_max)
{
    assert(n2_min < n2_max);
    int c[n2_max - n2_min];
    for (int i=0; i<(n2_max-n2_min); i++) c[i] = 0;
    
    int n = __atomic_load_n(&log_index, __ATOMIC_SEQ_CST);
    for (int i=0; i<n; i++)
        for (int x = n2_min; x<n2_max; x++) 
            if ((logs[i].id == id) && (logs[i].n1 == n1) && (logs[i].n2 == x)) {
                c[x-n2_min] = 1;
                break;
            }
    // TODO CHECK that all c are 1;
    for (int x=0; x<(n2_max - n2_min); x++)
        if (c[x] == 0) return 0;
    return 1;
}

int tlog_check_seq_n2(char id, int n1, int n2_min, int n2_max)
{
    assert(n2_min < n2_max);
    int c[n2_max - n2_min];
    for (int i=0; i<(n2_max-n2_min); i++) c[i] = 0;

    int n = __atomic_load_n(&log_index, __ATOMIC_SEQ_CST);
    int i = 0;
    for (int x = n2_min; x<n2_max; x++) 
        while (i<n) {
            if ((logs[i].id == id) && (logs[i].n1 == n1) && (logs[i].n2 == x)) {
                c[x-n2_min] = 1;
                i++;
                break;
            }
            i++;
        }
    
    // TODO CHECK that all c are 1;
    for (int x=0; x<(n2_max - n2_min); x++)
        if (c[x] == 0) return 0;
    return 1;
}

void tid_list_init(tid_list_t *list)
{
    list->array = NULL;
    list->size = 0;
    list->n = 0;
}

void tid_list_destroy(tid_list_t *list)
{
    free(list->array);
    list->size = 0;
    list->n = 0;
}

int tid_list_find(tid_list_t *list, pthread_t elem)
{
    for (int i=0; i<list->n; i++) {
        if (pthread_equal(elem, list->array[i])) 
            return i;
    }
    return -1;
}

int tid_list_insert_nodup(tid_list_t *list, pthread_t elem)
{
    // see if it already inside
    if (tid_list_find(list, elem) != -1)
        return 0; // not inserted
    
    if (list->n == list->size) {
        list->size += 10;
        list->array = (pthread_t *)reallocarray(list->array, list->size, sizeof(pthread_t));
        assert(list->array != NULL);
    }
    list->array[list->n++] = elem;
    
    return 1; // inserted 
}

void tlog_get_tid_list(tid_list_t *p)
{
    int n = __atomic_load_n(&log_index, __ATOMIC_SEQ_CST);
    
    for (int i=0; i<n; i++) {
        if (logs[i].tid_valid) 
            tid_list_insert_nodup(p, logs[i].tid);
    }
}

/*
  Checks that all threads execute in a "barrier" formation.
  Assume that every thread is organised in a loop

  while(cond) {
     wait start
     do work and tlog_log()
     signal end of work
  }
  and that there are a number N of such thread.

  Then, the following function checks that the thread actually block
  in the correct order, that is in group of N threads. Inside the
  group, they can execute in any order, however every thread is
  present exactly once in any group of N logs. 
*/

int tlog_check_thread_barrier(tid_list_t *list)
{
    // list->n is the number of threads
    // log_index is the number of logs
    if (list->n == 0) return 1;
    if (log_index < list->n) return 1;
    
    // Number of logs to look at 
    int nlogs = log_index - (log_index % list->n);
    
    // now let's see if the order is respected
    int li = 0;
    //printf("nlogs = %d\n", nlogs);
    while (li < nlogs) {
        // temp list 
        tid_list_t p;
        tid_list_init(&p);
        for (int i=0; i < list->n; i++) {
            if (logs[li].tid_valid) 
                if (tid_list_find(list, logs[li].tid) != -1)
                    tid_list_insert_nodup(&p, logs[li].tid);
            li++;
        }
        int x = p.n;
        
        tid_list_destroy(&p);
        
        if (x != list->n) {
            //print_err("x = %d   list->n = %d   li = %d", x, list->n, li);
            //            tlog_print();
            return 0;
        }
    }
    // everything is ok! 
    return 1;
}
