#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "csv.h"


/* Read a CSV header names line and store into headers[NUM_FIELDS]. */
int csv_read_header(FILE *f, char *headers[NUM_FIELDS]) {
    char line[MAX_LINE_LEN];
    char *fields[NUM_FIELDS];
    
    if (fgets(line, sizeof(line), f) && csv_parse_line(line, fields, NUM_FIELDS)) {
        // successfully read line and parsed line to fields
        for (int i = 0; i < NUM_FIELDS; i++) {
            // store the header names
            headers[i] = fields[i];
        }
        return 1;
    }
    return 0;
}


/* Parse one CSV address data line into separate strings to store in cols[]. */
int csv_parse_line(char *line, char *cols[], int expected_fields) {
    int col = 0;
    char *p = line;

    remove_newline(line);

    // continue until the end and reached expected number of fields
    while (*p != '\0' && col < expected_fields) {
        char field[MAX_FIELD_LEN];
        int field_len = 0;

        // quoted field
        if (*p == '"') {
            p++;

            while (*p != '\0') {
                if (*p == '"') {
                    // meets a double quote, skips
                    if (*(p + 1) == '"') {
                        field[field_len++] = '"';
                        p += 2;
                    } else {
                        // quoted field is ended
                        p++;
                        break;
                    }
                } else {
                    // skip characters inside quoted field
                    field[field_len++] = *p;
                    p++;
                }
            }
        } else {
            // when unquoted, read until reaches comma
            while (*p != '\0' && *p != ',') {
                field[field_len++] = *p;
                p++;
            }
        }

        // end of field and copy to array
        field[field_len] = '\0';
        cols[col] = strdup(field);
        assert(cols[col] != NULL);
        col++;

        // when read a comma, skip and proceed to the next field
        if (*p == ',') {
            p++;
        }
    }
    return 1;
}


/* Helper to remove any trailing '\n' characters after read. */
void remove_newline(char *s) {
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n')) {
        s[len - 1] = '\0';
        len--;
    }
}

