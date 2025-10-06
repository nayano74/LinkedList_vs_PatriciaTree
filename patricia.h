#ifndef _PATRICIA_H_
#define _PATRICIA_H_

#include <stdio.h>
#include "data.h"
#include "list.h"
#include "bit.h"

/* 
 * Node structure for patricia tree elements
 * prefix: char pointer to store bit-stem
 * prefixBits: number of bits in prefix
 * patricia_node *branch[2]: branch[0] for 0-bit, branch[1] for 1-bit
 * data: pointer to linked list structure where elements are records
   that correspond to this key
*/
typedef struct patricia_node {
    char *prefix;   
    unsigned int prefixBits; 
    struct patricia_node *branch[2];
    list_t *data;
} patricia_node_t;

/* 
 * Patricia tree structure
 * root: pointer to patricia_node strucutre acting as the root 
 * num_key: number of unique keys stored
*/
typedef struct patricia_tree {
    patricia_node_t *root;
    int num_key; 
} patricia_tree_t;

/* 
 * Strucutre for bit, node and string comparisons
 * bit_comps: number of bit comparisons made  
 * node_comps: number of node comparisons made
 * string_comps: number of string comparisons made
*/
typedef struct search_results {
    int bit_comps;
    int node_comps;
    int string_comps;
} search_results_t;

patricia_tree_t *create_patricia_tree();

void build_patricia_dictionary(FILE *inFile, patricia_tree_t *dictionary);

list_t *patricia_search_spell(patricia_tree_t *tree, const char *key, search_results_t *results);

void process_patricia_queries(patricia_tree_t *dict, FILE *output_file);

void free_patricia_tree(patricia_tree_t *tree, void (*data_free)(void *));
#endif

