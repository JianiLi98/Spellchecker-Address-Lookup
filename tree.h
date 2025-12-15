#ifndef _TREE_H_
#define _TREE_H_
#include "record.h"
#include "result.h"


#define START_BIT 0 // starting position of current bit


// Type definitions for tree dictionary.
typedef struct node_rec node_rec_t;
typedef struct tree_node tree_node_t;
typedef struct tree_dict tree_dict_t;

struct node_rec {
    record_t *rec;
    node_rec_t *next;
};

struct tree_node {
    char *prefix;
    int prefix_bits;
    tree_node_t *left;
    tree_node_t *right;
    node_rec_t *head; // stores records as linked list
    node_rec_t *tail;
};

struct tree_dict {
    tree_node_t *root;
    char *headers[NUM_FIELDS];
    size_t size;
};


/* Tree and node creation logic: */
/* Creates dictionary, and store NUM_FIELDS of header names read. */
tree_dict_t *create_tree_dict(char *headers[NUM_FIELDS]);

/* Create a new node with given prefix. */
tree_node_t *create_node(char *prefix, int prefix_bits);

/* Add a record to the node and store to a linked list. */
void add_record(tree_node_t *node, record_t *record);


/* Insertion logic: */
/* Helper to get key's total number of bits. */
int get_total_bits(char *key);

/* Compares the node’s stem/prefix (from bit 0) to the key starting at
 * key start_bit, bit by bit, and returns how many leading bits match
 */
int compare_prefix_bits(char *key, int start_bit, int key_total_bits,
    char *prefix, int prefix_bits);

/* Acessed by main driver for insertion into tree dict. */
void insert_tree(tree_dict_t *tree, char *key, record_t *record);

/* Inserts new key to tree by navigating tree, split on mismatch, or append record. */
tree_node_t *recursive_insert(tree_node_t *node, char *key, int total_bits, 
    int curr_bit, record_t *record);

/* Splits a node into a new parent node with common prefix with children
 * being the old node with shortened prefix and a new node for the key
 */
tree_node_t *split_node(tree_node_t *node, char *key, int total_bits, 
    int curr_bit, record_t *record, int match_count);


/* Exact match search logic: */
/* Searches for exact key and records last mismatch node for spellcheck. */
tree_node_t *exact_search(tree_dict_t *dict, char *key, result_t *result, 
    tree_node_t **mismatch_node);

/* Does exact match search on key down the tree.
 * If found exact match for key, append all records to result.
 * If not, set last matched node as marked for closest search.
 */
tree_node_t *recursive_exact_search(tree_node_t *node, char *key, int total_bits, 
    int curr_bit, result_t *result, tree_node_t **last_node);


/* Closest match search logic: */
/* Post traversal helper to collect all descendant records. */
void collect_subtree_records(tree_node_t *node, result_t *result);

/* Helper to extract EZI_ADD key from record. */
char *get_record_key(record_t *record);

/* Helper to check if candidate key has already been tested. */
int already_tested(result_t *result, int start, int i, char *candidate);

/* Choose best candidate key if it has 
 * minimum edit distance and alphabetically earliest
 */
char *find_best_key(result_t *res, char *query, int start, int cand_total);

/* Does closest-match search to find nearest key after mismatch. */
void search_closest(tree_node_t *last_match, char *key, result_t *result);


/* Free logic: */
/* Frees a subtree recursively. */
void free_node(tree_node_t *node);

/* Frees the entire tree dictionary structure. */
void free_tree(tree_dict_t *tree);


#endif
