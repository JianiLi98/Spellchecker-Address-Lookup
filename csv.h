#ifndef _CSV_H_
#define _CSV_H_
#include <stdio.h>
#include "record.h"


#define MAX_LINE_LEN 512  // Max input record length (including '\n'/'\0')
#define MAX_FIELD_LEN 128 // Max characters per field (including '\0')


/* Read a CSV header names line and store into headers[NUM_FIELDS]. */
int csv_read_header(FILE *f, char *headers[NUM_FIELDS]);

/* Parse one CSV address data line into separate strings to store in cols[]. */
int csv_parse_line(char *line, char *cols[], int expected_fields);

/* Remove any trailing '\n' characters after read. */
void remove_newline(char *s);


#endif
