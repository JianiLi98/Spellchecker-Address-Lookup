#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "tree.h"
#include "bit.h"
#include "edit_dist.h"


/* Creates dictionary, and store NUM_FIELDS of header names read. */
tree_dict_t *create_tree_dict(char *headers[NUM_FIELDS]) {
    tree_dict_t *dict = (tree_dict_t*)malloc(sizeof(*dict));
    assert(dict);
    
    dict->root = NULL;
    dict->size = 0;
    
    for (int i = 0; i < NUM_FIELDS; i++) {
        dict->headers[i] = headers[i];
    }
    return dict;
}


/* Create a new node with given prefix. */
tree_node_t *create_node(char *prefix, int prefix_bits) {
    tree_node_t *node = (tree_node_t*)malloc(sizeof(*node));
    assert(node);

    node->prefix = prefix;
    node->prefix_bits = prefix_bits;
    node->left = node->right = NULL;
    node->head = node->tail = NULL;

    return node;
}


/* Add a record to the node and store to a linked list. */
void add_record(tree_node_t *node, record_t *record) {
    node_rec_t *node_rec = (node_rec_t *)malloc(sizeof(*node_rec));
    assert(node_rec);
    node_rec->rec = record;
    node_rec->next = NULL;

    if (!node->head) {
        node->head = node->tail = node_rec;
    } else {
        node->tail = node->tail->next = node_rec;
    }
}


/* Helper to get key's total number of bits. */
int get_total_bits(char *key) {
    return (strlen(key) + 1) * BITS_PER_BYTE;
}


/* Compares the node’s stem/prefix (from bit 0) to the key starting at
 * key start_bit, bit by bit, and returns how many leading bits match
 */
int compare_prefix_bits(char *key, int start_bit, int key_total_bits, 
        char *prefix, int prefix_bits) {

    int remaining = key_total_bits - start_bit;
    if (remaining < 0) remaining = 0;
    
    int limit = remaining;
    if (prefix_bits < remaining) {
        limit = prefix_bits;
    }
    
    for (int i = 0; i < limit; i++) {
        // found mismatching bit
        if (getBit(key, start_bit + i) != getBit(prefix, i)) {
            return i;
        }
    }
    return limit;
}


/* Acessed by main driver for insertion into tree dict. */
void insert_tree(tree_dict_t *tree, char *key, record_t *record) {
    int total_bits = get_total_bits(key);
    tree->root = recursive_insert(tree->root, key, total_bits, START_BIT, record);
    tree->size++;
}


/* Inserts new key to tree by navigating tree, split on mismatch/append record. */
tree_node_t *recursive_insert(tree_node_t *node, char *key, int total_bits, 
        int curr_bit, record_t *record) {
    
    // Create new leaf for remaining suffix if not in tree already
    if (node == NULL) {
        int remaining_bits = total_bits - curr_bit;
        char *remaining_pref = createStem(key, curr_bit, remaining_bits);
        tree_node_t *new_node = create_node(remaining_pref, remaining_bits);
        add_record(new_node, record);

        return new_node;
    }

    // Compare key to prefix and count bits to see if match or mismatch
    int match_count = compare_prefix_bits(key, curr_bit, total_bits, 
        node->prefix, node->prefix_bits);

    // Case 1: mismatch or more bits in node than key
    if (match_count < node->prefix_bits) {
        // do split node and get a common prefix parent
        return split_node(node, key, total_bits, curr_bit, record, match_count);
    }

    // Case 2: all match prefix
    curr_bit += node->prefix_bits;

    if (curr_bit >= total_bits) {
        // found matching node and add record
        add_record(node, record);
        return node;
    }

    // still more bits and continue down the tree
    int next_key_bit = getBit(key, curr_bit);
    if (next_key_bit == 0) {
        node->left = recursive_insert(node->left, key, total_bits, 
            curr_bit, record);
    } else {
        node->right = recursive_insert(node->right, key, total_bits, 
            curr_bit, record);
    }
    return node;
}


/* Splits a node into a new parent node with common prefix with children
 * being the old node with shortened prefix and a new node for the key
 */
tree_node_t *split_node(tree_node_t *node, char *key, int total_bits, 
        int curr_bit, record_t *record, int match_count) {
    
    // Create a new parent node with common prefix
    char *common_pref = createStem(node->prefix, START_BIT, match_count);
    tree_node_t *parent = create_node(common_pref, match_count);

    // Adjust old node's prefix
    int old_remaining_bits = node->prefix_bits - match_count;
    char *old_pref = createStem(node->prefix, match_count, old_remaining_bits);
    free(node->prefix);
    node->prefix = old_pref;
    node->prefix_bits = old_remaining_bits;

    // Create new node for key
    int new_start_bit = curr_bit + match_count;
    int new_remaining_bits = total_bits - new_start_bit;
    char *new_pref = createStem(key, new_start_bit, new_remaining_bits);
    tree_node_t *new_node = create_node(new_pref, new_remaining_bits);
    add_record(new_node, record);

    // Allocate old and new child to either side of common parent node
    int old_start = getBit(node->prefix, START_BIT);
    int new_start = getBit(key, new_start_bit);
    assert(old_start != new_start);

    if (old_start == 0) {
        parent->left = node;
        parent->right = new_node;
    } else {
        parent->left = new_node;
        parent->right = node;
    }

    return parent;
}


/* Searches for exact key and records last mismatch node for spellcheck. */
tree_node_t *exact_search(tree_dict_t *dict, char *key, result_t *result, 
        tree_node_t **mismatch_node) {
    
    *mismatch_node = NULL;
    if (!dict || !dict->root) return NULL;
    return recursive_exact_search(dict->root, key, get_total_bits(key), 
        START_BIT, result, mismatch_node);
}


/* Does exact match search on key down the tree.
 * If found exact match for key, append all records to result.
 * If not, set last matched node as marked for closest search.
 */
tree_node_t *recursive_exact_search(tree_node_t *node, char *key, int total_bits, 
        int curr_bit, result_t *result, tree_node_t **last_node) {
    
    // Not found case
    if (node == NULL) {
        return NULL;
    }

    // Compare key and prefix and keep track of comparison counts
    result->node_cmps ++;
    int match_count = compare_prefix_bits(key, curr_bit, total_bits, 
        node->prefix, node->prefix_bits);
    result->bit_cmps += match_count;
    
    // No exact match for key search case
    if (match_count < node->prefix_bits) {
        result->bit_cmps++;
        *last_node = node;
        return NULL;
    }

    // Track of the last fully-matched node for closest search
    *last_node = node;

    // Found exact match case
    curr_bit += node->prefix_bits;
    if (curr_bit >= total_bits) {
        result->str_cmps++;
        // reaches key's end bit and add in to result
        for (node_rec_t *nrec = node->head; nrec; nrec = nrec->next) {
            result->matches[result->match_count++] = nrec->rec;
        }
        return node;
    }

    // Continue down the tree
    int key_next_bit = getBit(key, curr_bit);
    if (key_next_bit == 0) {
        return recursive_exact_search(node->left, key, total_bits, 
            curr_bit, result, last_node);
    } else {
        return recursive_exact_search(node->right, key, total_bits, 
            curr_bit, result, last_node);
    }
}


/* Post traversal helper to collect all descendant records. */
void collect_subtree_records(tree_node_t *node, result_t *result) {
    if (!node) return;
    for (node_rec_t *nrec = node->head; nrec; nrec = nrec->next) {
        result->matches[result->match_count++] = nrec->rec;
    }
    collect_subtree_records(node->left,  result);
    collect_subtree_records(node->right, result);
}


/* Helper to extract EZI_ADD key from record. */
char *get_record_key(record_t *record) {
    return record->fields[EZI_ADD_INDEX];
}


/* Helper to check if candidate key has already been tested. */
int already_tested(result_t *result, int init_count, int i, char *candidate) {
    for (int j = init_count; j < i; j++) {
        if (strcmp(get_record_key(result->matches[j]), candidate) == 0) {
            return 1;
        }
    }
    return 0;
}


/* Choose best candidate key if it has 
 * minimum edit distance and alphabetically earliest
 */
char *find_best_key(result_t *result, char *key, int init_count, int cand_count) {
    char *best_key = NULL;
    int best_dist = 0;

    for (int i = init_count; i < init_count + cand_count; i ++) {
        char *candidate = get_record_key(result->matches[i]);
        // skip if already seen and tested this key
        if (already_tested(result, init_count, i, candidate)) continue;

        int dist = editDistance(key, candidate, strlen(key), strlen(candidate));
        // update best candidate if min edit dist and alphabetically first
        if (!best_key || dist < best_dist || 
            (dist == best_dist && strcmp(candidate, best_key) < 0)) {
            best_key = candidate;
            best_dist = dist;
        }
    }
    return best_key;
}


/* Does closest-match search to find nearest key after mismatch. */
void search_closest(tree_node_t *last_match, char *key, result_t *result) {
    if (!last_match) return;
    
    // Collect all descendants as possible candidates
    int init_count = result->match_count;
    collect_subtree_records(last_match, result);
    int cand_count = result->match_count - init_count;
    if (!cand_count) {
        result->match_count = init_count;
        return;
    }

    char *best_cand = find_best_key(result, key, init_count, cand_count);
    
    // Store all records with the best candidate key to result
    int best_count = init_count;
    for (int i = init_count; i < (init_count + cand_count); i ++) {
        if (strcmp(get_record_key(result->matches[i]), best_cand) == 0) {
            result->matches[best_count++] = result->matches[i];
        }
    }
    result->match_count = best_count;

    result->str_cmps++;
}


/* Frees a subtree recursively. */
void free_node(tree_node_t *node) {
    if (!node) return;
    free_node(node->left);
    free_node(node->right);
    
    free(node->prefix);

    node_rec_t *curr = node->head;
    while (curr) {
        node_rec_t *tmp = curr->next;
        free_record(curr->rec);
        free(curr);
        curr = tmp;
    }

    free(node);
}


/* Frees the entire tree dictionary structure. */
void free_tree(tree_dict_t *tree) {
    assert(tree);
    free_node(tree->root);
    
    // Free headers
    for (int i = 0; i < NUM_FIELDS; i++) {
        free(tree->headers[i]);
    }
    
    free(tree);
}
