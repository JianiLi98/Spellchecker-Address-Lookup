#include "record.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


/* Creates and returns an empty record. */
record_t *create_record(char *cols[NUM_FIELDS]) {
    record_t *rec = (record_t *)malloc(sizeof(*rec));
    assert(rec);

    for (int i = 0; i < NUM_FIELDS; i ++) {
        rec->fields[i] = cols[i];
    } 
    return rec;
}


/* Frees a record and all the strings. */
void free_record(record_t *rec) {
    for (int i = 0; i < NUM_FIELDS; i++) {
        free(rec->fields[i]);
    }
    free(rec);
}


/* Prints the address record in the required format,
 * where x/y-coordinate fields are rounded and printed to 5 decimals.
 */
void print_record(FILE *f, record_t *rec, char *headers[NUM_FIELDS]) {
    fprintf(f, "--> ");

    for (int i = 0; i < NUM_FIELDS; i++) {
        fprintf(f, "%s: ", headers[i]);

        // round and print x and y-coords to 5 decimal places
        if ((i == X_COORD_INDEX || i == Y_COORD_INDEX)) {
            print_rounded_coordinates(f, rec->fields[i]);
        } else {
            // print other fields as strings
            fputs(rec->fields[i], f);
        }

        fprintf(f, " || ");
    }
    fprintf(f, "\n");
}


/* Helper used by print_record to convert and print x/y rounded to 5 decimal places. */
void print_rounded_coordinates(FILE *f, char *str) {
    if (str) {
        char *end_ptr = NULL;
        long double coord = strtold(str, &end_ptr);
        if (end_ptr != str) {
            // conversion is successful
            fprintf(f, "%.5Lf", coord);
            return;
        }
    }
    // if unsuccessful, print original string
    fputs(str, f);
}
