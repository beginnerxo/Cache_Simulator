/*
FOR UNIQUE CACHE 
GROUP MEMBERS: 
    Ryan Xu (rwx1)
    Alex Oteng (afo35)
    Harjas Rangi (hkr23)
    Humza Nawaz (hsn13)
    Pariksheet Naik
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
        even_cache[i] = (cache_line_t*) malloc (sizeof(cache_line_t) * E);
        odd_cache[i] = (cache_line_t*) malloc (sizeof(cache_line_t) * E);
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

*/
void replaceMostUsed(mem_addr_t tag) {
    int min = 0;
    for (int i = 0; i < MOST_USED_SIZE; i++) {
        if (most_used_cache[i].access_count < most_used_cache[min].access_count) {
            min = i;
        }
    }

    most_used_cache[min].tag = tag;
    most_used_cache[min].valid = 1;
    most_used_cache[min].lru = lru_counter + 1;
    most_used_cache[min].access_count = 1;
}

/*

*/
void accessData(mem_addr_t addr) {
    unsigned long long int eviction_lru = ULONG_MAX;
    unsigned int eviction_line = 0;
    mem_addr_t set_index = (addr >> b) & set_index_mask;
    mem_addr_t tag = addr >> (s + b);

    cache_set_t cache_set;
    if (addr % 2 == 0) {
        cache_set = even_cache[set_index];
    } else {
        cache_set = odd_cache[set_index];
    }

    for (int i = 0; i < E; i++) {
        if (cache_set[i].tag == tag && cache_set[i].valid == 1) {
            hit_count++;
            if (verbosity) {
                printf("hit ");
            }
            cache_set[i].lru = lru_counter + 1;
            cache_set[i].access_count++;
            replaceMostUsed(tag);
            return;
        }
    }

    miss_count++;
    if (verbosity) {
        printf("miss ");
    }
    for (int i = 0; i < E; i++) {
        if (cache_set[i].lru < eviction_lru){
            eviction_line = i;
            eviction_lru = cache_set[i].lru;
        }
    }

    if (cache_set[eviction_line].valid) {
        eviction_count++;
        if (verbosity) {
            printf("eviction ");
        }
    }

    cache_set[eviction_line].valid = 1;
    cache_set[eviction_line].tag = tag;
    cache_set[eviction_line].lru = lru_counter + 1;
    cache_set[eviction_line].access_count = 1;
    replace_most_used(tag);
}

/*

*/
void replayTrace(char* trace_fn) {
    char buf[1000];
    mem_addr_t addr = 0;
    unsigned int len = 0;
    FILE* trace_fp = fopen(trace_fn, "r");

    if (!trace_fp){
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while (fgets(buf, 1000, trace_fp) != NULL) {
        if (buf[1]=='S' || buf[1]=='L' || buf[1]=='M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);
      
            if (verbosity) {
                printf("%c %llx,%u ", buf[1], addr, len);
            }
            accessData(addr);

            /* If the instruction is R/W then access again */
            if (buf[1]=='M') {
                accessData(addr);
            }
            if (verbosity) {
                printf("\n");
            }
        }
    }

    fclose(trace_fp);
}

/*

*/
void printUsage(char* argv[]) {
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}



/*
 * main - Main routine 
 */
int main(int argc, char* argv[])
{
	char c;

    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
        switch(c){
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace_file = optarg;
            break;
        case 'v':
            verbosity = 1;
            break;
        case 'h':
            printUsage(argv);
            exit(0);
        default:
            printUsage(argv);
            exit(1);
        }
    }

    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }

    /* Compute S, E and B from command line args */
    S = (unsigned int) pow(2, s);
    B = (unsigned int) pow(2, b);
 
    /* Initialize cache */
    initCache();

#ifdef DEBUG_ON
    printf("DEBUG: S:%u E:%u B:%u trace:%s\n", S, E, B, trace_file);
    printf("DEBUG: set_index_mask: %llu\n", set_index_mask);
#endif
 
    replayTrace(trace_file);

    /* Free allocated memory */
    freeCache();

    /* Output the hit and miss statistics for the autograder */
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
