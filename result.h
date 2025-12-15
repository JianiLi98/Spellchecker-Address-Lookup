#ifndef RESULT_H
#define RESULT_H
#include "record.h"


#define MISMATCH_BIT 1 // additional count for the mismatching bit


// type definition for searched matching results
typedef struct {
    record_t **matches;
    int match_count;
    int bit_cmps;
    int node_cmps;
    int str_cmps; 
} result_t;


/* Initialises an empty result with a matches array,
 * maximum capacity is dict->size. 
 */
void initialise_result(result_t *r, size_t match_capacity);

/* Frees the matches array and reset counters to 0. */
void free_result(result_t *r);

/* Creates an empty result_t and initialises. */
result_t *create_result(size_t match_capacity);

/* Appends a matching record pointer into the result. */
void result_add_match(result_t *r, record_t *rec);

/* Counts number of bits compared in the first mismatching byte.
 * Used as a helper in compare_strings_bits that returns
 * the first matching bits count + one mismatch bit count.
 */
int bits_in_mismatch(char *s1, char *s2, int byte_idx);

/* Compares strings and counts bit accesses and comparisons.
 * Returns negative/zero/positive.
 */
int compare_strings_bits(char *s1, char *s2, int *bit_count);


#endif
