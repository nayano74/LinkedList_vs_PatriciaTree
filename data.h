/* data.h
 *
 * Header file for handling of Victorian address data.
 * Defines structures and functions for reading, storing, and processing
 * address records in CSV files with 35 fields.
 */


#ifndef _DATA_H_
#define _DATA_H_

#include <stdio.h>
#include "list.h"

#define FIELD_COUNT 35

#define MAX_LINE_LENGTH 512
#define X_POS 33
#define Y_POS 34

typedef struct { // contains all the fields from each row in the csv 
    char *fields[FIELD_COUNT];    
} address_t;

address_t *data_read(FILE *input_file);

const char *address_get_key(const void *address);

void address_print_file(FILE *output_file, void *address);

void address_free(void *address);

int parse_line(char *line, char *fields[], int max_fields);

void buildDictionary(FILE *f, list_t *dictionary);

void output_results(list_t *dict, FILE *output_file);

void chomp(char *s);
#endif

