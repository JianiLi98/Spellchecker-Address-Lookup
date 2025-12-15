#ifndef _RECORD_H_
#define _RECORD_H_
#include <stdio.h>


#define NUM_FIELDS 35    // number of columns/fields for csv
#define EZI_ADD_INDEX 1  // index position for field EZI_ADD
#define X_COORD_INDEX 33 // index position for field x-coordinate
#define Y_COORD_INDEX 34 // index position for field y-coordinate


// data type definition for an address record
typedef struct {
    char *fields[NUM_FIELDS];
} record_t;


/* Creates and returns an empty record. */
record_t *create_record(char *cols[NUM_FIELDS]);

/* Frees a record and all the strings. */
void free_record(record_t *rec);

/* Prints the address record in the required format,
 * where x/y-coordinate fields are rounded and printed to 5 decimals.
 */
void print_record(FILE *f, record_t *rec, char *headers[NUM_FIELDS]);

/* Helper used by print_record to convert and print x/y rounded to 5 decimal places. */
void print_rounded_coordinates(FILE *f, char *str);


#endif
