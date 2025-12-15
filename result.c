#include "result.h"
#include "bit.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


/* Initialises an empty result with a matches array,
 * maximum capacity is dict->size. 
 */
void initialise_result(result_t *r, size_t match_capacity) {
    r->matches = NULL;
    r->match_count = 0;
    r->bit_cmps = r->node_cmps = r->str_cmps = 0;

    if (match_capacity > 0) {
        r->matches = (record_t**)malloc(match_capacity * sizeof(record_t*));
    }
}


/* Frees the matches array and reset counters to 0. */
void free_result(result_t *r) {
    free(r->matches);
    r->matches = NULL;
    r->match_count = 0;
    r->bit_cmps = r->node_cmps = r->str_cmps = 0;
}


/* Creates an empty result_t and initialises. */
result_t *create_result(size_t match_capacity) {
    result_t *result = (result_t*)malloc(sizeof(*result));
    assert(result);
    initialise_result(result, match_capacity);
    
    return result;
}


/* Appends a matching record pointer into the result. */
void result_add_match(result_t *r, record_t *rec) {
    r->matches[r->match_count++] = rec;
}


/* Counts number of bits compared in the first mismatching byte.
 * Used as a helper in compare_strings_bits that returns
 * the first matching bits count + one mismatch bit count.
 */
int bits_in_mismatch(char *s1, char *s2, int byte_idx) {
    int start_bit = byte_idx * BITS_PER_BYTE;
    
    // start scanning from left to right within the byte
    for (int i = 0; i < BITS_PER_BYTE; i++) {
        int b1 = getBit(s1, start_bit + i);
        int b2 = getBit(s2, start_bit + i);
        // stop when found the first mismatching bit
        if (b1 != b2) {
            // include the mismatch bit in the returned count
            return i + MISMATCH_BIT;
        }
    }
    // make sure to return some int as fall back
    return BITS_PER_BYTE;
}


/* Compares strings and counts bit accesses and comparisons.
 * Returns negative/zero/positive.
 */
int compare_strings_bits(char *s1, char *s2, int *bit_count) {
    int byte_idx = 0;

    while (s1[byte_idx] == s2[byte_idx]) {
        unsigned char c = (unsigned char)s1[byte_idx];
        *bit_count += BITS_PER_BYTE;
        // count the null byte
        if (c == '\0') {
            // strings are equal
            return 0;
        }
        byte_idx++;
    }

    // encounter first mismatch, get bit count in mismatch char
    unsigned char c1 = (unsigned char)s1[byte_idx];
    unsigned char c2 = (unsigned char)s2[byte_idx];
    *bit_count += bits_in_mismatch(s1, s2, byte_idx);

    return (int)c1 - (int)c2;
}
