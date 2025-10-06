/* list.h
 *
 * Header file for linked list dictionary implementation.
 * Provides structures and function declarations for a generic linked list
 * that can store any type of data through void pointers.
 */

#ifndef _DICT_H_
#define _DICT_H_

/* 
 * Node structure for linked list elements
 * data: void pointer to store any type of data
 * next: pointer to the next node in the list
*/
typedef struct node node_t;

struct node {
    void *data;
    node_t *next;
};

/*
 * Linked list structure
 * head: pointer to the first node in the list
 * tail: pointer to the last node in the list
 * num_node: count of nodes currently in the list
*/
typedef struct list {
    node_t *head;
    node_t *tail;
    int num_node;
} list_t;

list_t *create_list();

void insert_record(list_t *list, void *data);

list_t *search_list(list_t *list, char *key, int *count, int *comparisons, 
                    const char *(*data_get_key)(const void *));

void free_list(list_t *list, void (*data_free)(void *));

#endif
