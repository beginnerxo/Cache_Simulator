/*
FOR UNIQUE CACHE 
GROUP MEMBERS: 
#FILL IN

LOCATION: Rutgers University
CLASS: CS211 Computer Architecture
SESSION: Fall 2024
PROFESSOR: Dr. Tina Burns

DESCRIPTION:


NOTES:
 */
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include "cachelab.h"

//#define DEBUG_ON 
#define ADDRESS_LENGTH 64

// Defines the size of the most frequently used section
#define MOST_USED_SIZE 4

/* Type: Memory address */
typedef unsigned long long int mem_addr_t;

/* Type: Cache line */
typedef struct cache_line {
    char valid;
    mem_addr_t tag;
    unsigned long long int lru;
    unsigned long long int access_count; // Used for most frequently used logic
} cache_line_t;

typedef cache_line_t* cache_set_t;
typedef cache_set_t* cache_t;

/* Globals set by command line args */
int verbosity = 0; /* print trace if set */
int s = 0; /* set index bits */
int b = 0; /* block offset bits */
int E = 0; /* associativity */
char* trace_file = NULL;

/* Derived from command line args */
int S; /* number of sets */
int B; /* block size (bytes) */

/* Counters used to record cache statistics */
int miss_count = 0;
int hit_count = 0;
int eviction_count = 0;
unsigned long long int lru_counter = 1;

/* The cache we are simulating */
cache_t even_cache;
cache_t odd_cache;
cache_line_t most_used_cache[MOST_USED_SIZE];
mem_addr_t set_index_mask;

// Initializes the caches
void initCache() {
    even_cache = (cache_set_t*) malloc(sizeof(cache_set_t) * S);
    odd_cache = (cache_set_t*) malloc(sizeof(cache_set_t) * S);
    for (int i = 0; i < S; i++) {
        even_cache[i] = (cache_line_t*) malloc(sizeof(cache_line_t) * E);
        odd_cache[i] = (cache_line_t*) malloc(sizeof(cache_line_t) * E);
        for (int j = 0; j < E; j++) {
            even_cache[i][j].valid = 0;
            odd_cache[i][j].valid = 0;
            even_cache[i][j].tag = 0;
            odd_cache[i][j].tag = 0;
            even_cache[i][j].lru = 0;
            odd_cache[i][j].lru = 0;
            even_cache[i][j].access_count = 0;
            odd_cache[i][j].access_count = 0;
        }
    }
    for (int i = 0; i < MOST_USED_SIZE; i++) {
        most_used_cache[i].valid = 0;
        most_used_cache[i].tag = 0;
        most_used_cache[i].lru = 0;
        most_used_cache[i].access_count = 0;
    }
    set_index_mask = (mem_addr_t)(pow(2, s) - 1);
}

// Frees allocated memory
void freeCache() {
    for (int i = 0; i < S; i++) {
        free(even_cache[i]);
        free(odd_cache[i]);
    }
    free(even_cache);
    free(odd_cache);
}


/*
 * main - Main routine 
 */
int main(int argc, char* argv[])
{
	//Fill in code here
    
    return 0;
}
