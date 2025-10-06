/* list.c
 *
 * Implementation of the dicitonary using linked list.
 *
 * Includes functions to create the list
 * - Insert record into the list
 * - Search the list using a key and a provided function to find the key in each nodes data.
 * - Free the list and nodes using a function to free the data 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "list.h"
#include "bit.h"

/*
 * Creates a new list using malloc
 * includes a head, tail and number of nodes in the list
 * list is intialised as NULL, with number of nodes being 0
 * returns a pointer to the list 
 */
list_t *create_list() {
    list_t *list = malloc(sizeof(list_t));
    assert(list);

    list->head = NULL;
    list->tail = NULL;
    list->num_node = 0;

    return list;
}

/*
 * Inserts a data into the list 
 * data is set as pointer to void, to make the code modular
 * creates a node using malloc then inserts the data, Inserts
 * data into list and then increments counter by 1
 */
void insert_record(list_t *list, void *data) {
    assert(list && data);
    
    node_t *node = malloc(sizeof(*node)); // creating node
    assert(node);
    node->data = data;
    node->next = NULL;

    if (list->head == NULL) { //checking if list is empty
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }

    list->num_node++;
}

/*
 * Function to search through a linked list and find all nodes that match the given key.
 * It counts comparisons at various levels: bit-level comparison, node accesses, and string comparisons.
 * Returns a new list with the matching elements from the original list.
 */
list_t *search_list(list_t *list, char *key, int *count, int *comparisons, const char *(*data_get_key)(const void *)) {
    assert(list && key);
    int count_func = 0;

    //Initialise comparison counters
    int bit_comparisons = 0;
    int node_comparisons = 0;
    int string_comparisons = 0;

    list_t *new_list = create_list(); // create list for matches
    assert(new_list);
    
    for(node_t *cur = list->head; cur != NULL; cur = cur->next) {
        node_comparisons++; // Count each node access

        const char *cur_key = data_get_key(cur->data);
        if (cur_key) {
            string_comparisons++; // Count each string comparison

            bit_comparisons += count_bit_comparisons(key, cur_key); // compares strings by bits

            if (strcmp(cur_key, key) == 0) {
                insert_record(new_list, cur->data); // inserts matching record to list
                count_func++;
            }
        }
    }

    if (count) {
        *count = count_func;
    }

    if (comparisons) { // updating the comparisons 
        comparisons[0] = bit_comparisons;
        comparisons[1] = node_comparisons;
        comparisons[2] = string_comparisons;
    }
    return new_list;
}

/*
 * Frees the linked list and also frees the data using data free function
 */
void free_list(list_t *list, void (*data_free)(void *)) {
    assert(list);
    
    node_t *cur = list->head;
    while(cur){
        node_t *tmp = cur;
        cur = cur->next;

        if(data_free) { // checks if data free fucntion is provided
            data_free(tmp->data);
        }

        free(tmp);
    }

    free(list);
}


