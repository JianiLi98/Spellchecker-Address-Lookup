#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tree.h"
#include "record.h"
#include "result.h"
#include "csv.h"


tree_dict_t *build_tree_dict(FILE *in_fp);
void process_search(FILE *input_in, FILE *out_fp, tree_dict_t *tree_dict);
void print_result_outfile(FILE *out_fp, tree_dict_t *tree_dict, result_t *res);
void print_result_stdout(char *input_EZI_ADD, result_t *result);


int main(int argc, char *argv[]) {
    // check if valid arguments
    if (argc != 4) {
        return 1;
    }
    if (strcmp(argv[1], "2") != 0) {
        return 1;
    }

    FILE *in_fp = fopen(argv[2], "r");
    if (!in_fp) { return 1; }
    FILE *out_fp = fopen(argv[3], "w");
    if (!out_fp) { fclose(in_fp); return 1; }

    tree_dict_t *tree_dict = build_tree_dict(in_fp);
    fclose(in_fp);

    process_search(stdin, out_fp, tree_dict);

    fclose(out_fp);
    free_tree(tree_dict);
    return 0;
}


/* Reads CSV headers and records, inserts them into a Patricia tree.
 * Returns pointer to the tree dictionary.
 */
tree_dict_t *build_tree_dict(FILE *in_fp) {
    char *headers[NUM_FIELDS];
    tree_dict_t *tree_dict = NULL;
    if (csv_read_header(in_fp, headers)) {
        // create dictionary and store header if read successful
        tree_dict = create_tree_dict(headers);
    }

    char line[MAX_LINE_LEN];
    while (fgets(line, sizeof(line), in_fp)) {
        char *fields[NUM_FIELDS];
        if (csv_parse_line(line, fields, NUM_FIELDS)) {
            // create and store successfully read and parsed address record
            record_t *rec = create_record(fields);
            insert_tree(tree_dict, rec->fields[EZI_ADD_INDEX], rec);
        }
    }
    return tree_dict;
}


/* Implements key search from stdin and searches the tree
 * write results to output file and stdout. 
 */
void process_search(FILE *input_in, FILE *out_fp, tree_dict_t *tree_dict) {
    char input_EZI_ADD[MAX_LINE_LEN];

    // while still reading in search key
    while (fgets(input_EZI_ADD, sizeof(input_EZI_ADD), input_in)) {
        remove_newline(input_EZI_ADD);
        if (input_EZI_ADD[0] == '\0') continue;

        fprintf(out_fp, "%s\n", input_EZI_ADD);

        result_t *result = (result_t *)malloc(sizeof(*result));
        assert(result);
        initialise_result(result, tree_dict->size);
        
        // search the tree and try to find exact match
        tree_node_t *mismatch_node = NULL;
        tree_node_t *found_node = recursive_exact_search(tree_dict->root, input_EZI_ADD,
            get_total_bits(input_EZI_ADD), START_BIT, result, &mismatch_node);
        
        // if not exact match, find closest match
        if (!found_node && mismatch_node) {
            search_closest(mismatch_node, input_EZI_ADD, result);
        }

        // Write to output file and stdout
        print_result_outfile(out_fp, tree_dict, result);
        print_result_stdout(input_EZI_ADD, result);

        free_result(result);
        free(result);
    }
}


/* Helper to print NOTFOUND or matching records to output file. */
void print_result_outfile(FILE *out_fp, tree_dict_t *tree_dict, result_t *result) {
    if (result->match_count == 0) {
        fputs("NOTFOUND\n", out_fp);
    } else {
        for (int i = 0; i < result->match_count; i ++) {
            print_record(out_fp, result->matches[i], tree_dict->headers);
        }
    }
}


/* Helper to format and print matching records and comparison results to stdout. */
void print_result_stdout(char *input_EZI_ADD, result_t *result) {
    printf("%s --> %d records found - comparisons: b%d n%d s%d\n", 
            input_EZI_ADD, result->match_count, result->bit_cmps, 
            result->node_cmps, result->str_cmps);
}
