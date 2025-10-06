/* data.c
 *
 * Implementation of Victorian address data processing functions.
 * Handles CSV file parsing, address record management, and search query processing.
 * Supports quoted fields and basic CSV escape sequences for robust data handling.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "data.h"
#include "list.h"

/*
 * Gets the key of the address
*/
const char *address_get_key(const void *address) {
    //EZI_ADD at second field
    const address_t *addr = (const address_t *)address;

    if (addr != NULL && addr->fields[1] != NULL) {
        return addr->fields[1];
    }

    return "";
}

/*
 * Prints the record to the output file
*/
void address_print_file(FILE* output_file, void *address) {
    address_t *addr = (address_t *)address;

    //Field names for output
    char *fields[] = {
        "PFI", "EZI_ADD", "SRC_VERIF", "PROPSTATUS", "GCODEFEAT", "LOC_DESC",
        "BLGUNTTYP", "HSAUNITID", "BUNIT_PRE1", "BUNIT_ID1", "BUNIT_SUF1",
        "BUNIT_PRE2", "BUNIT_ID2", "BUNIT_SUF2", "FLOOR_TYPE", "FLOOR_NO_1",
        "FLOOR_NO_2", "BUILDING", "COMPLEX", "HSE_PREF1", "HSE_NUM1", "HSE_SUF1",
        "HSE_PREF2", "HSE_NUM2", "HSE_SUF2", "DISP_NUM1", "ROAD_NAME", "ROAD_TYPE",
        "RD_SUF", "LOCALITY", "STATE", "POSTCODE", "ACCESSTYPE", "x", "y"
    };

    fprintf(output_file, "--> ");

    for (int i=0; i < FIELD_COUNT; i++) {
        if (i == X_POS || i == Y_POS) {
            double x = atof(addr->fields[i]);
            fprintf(output_file, "%s: %.5lf || ", fields[i], x);
        } else {
            fprintf(output_file, "%s: %s || ", fields[i], addr->fields[i]);
        }
    }
    fprintf(output_file, "\n");
}

/*
 * Frees an address record
*/
void address_free(void *address) {
    assert(address);

    for(int i = 0; i < FIELD_COUNT; i++) {
        free(((address_t *)address)->fields[i]);
    }
    free(address);
}

/*
 * Parse CSV line into field array
*/
int parse_line(char *line, char *fields[], int max_fields) {
    int field_count = 0;
    char *p = line;
    int in_quotes = 0;
    char *start = p;

    // State machine to handle CSV parsing with quoted fields
    while (*p && field_count < max_fields) {
        if (*p == '"' && !in_quotes) {
            // Enter quoted mode, skip quote
            in_quotes = 1;
            start = ++p;
        } else if (*p == '"' && in_quotes) {
            // Closing quote, terminate and move on
            in_quotes = 0;
            *p++ = '\0';
            fields[field_count++] = start;
            // Skip comma after quoted field
            if (*p == ',') p++;
            start = p;
        } else if (*p == ',' && !in_quotes) {
            // End of unquoted field
            *p = '\0';
            fields[field_count++] = start;
            p++;
            start = p;
        } else {
            p++;
        }
    }

    // Last field
    if (field_count < max_fields && start <= p) {
        fields[field_count++] = start;
    }

    return field_count;
}

/*
 * Reads a single line from the input CSV and returns a pointer to an address_t struct
*/
address_t *data_read(FILE *input_file) {
    static int header_read = 0;
    char line[MAX_LINE_LENGTH];
    char *fields[FIELD_COUNT];

    // Skip header line upon first call
    if (!header_read) {
        if (fgets(line, sizeof(line), input_file) == NULL) {
            return NULL;
        }
        header_read = 1;
    }

    // Read line
    if (fgets(line, sizeof(line), input_file) == NULL) {
        return NULL;
    }

    // Remove newline character if present
    char *newline = strchr(line, '\n');
    if (newline) {
        *newline = '\0';
    }

    // Pase csv line into temporary fields
    int field_count = parse_line(line, fields, FIELD_COUNT);

    // Allocate address struct
    address_t *addr = malloc(sizeof(*addr));
    assert(addr);

    // Copy strings
    for (int i = 0; i < FIELD_COUNT; i++) {
        const char *src = (i < field_count && fields[i]) ? fields[i] : "";
        size_t len = strlen(src) + 1;          // +1 for '\0'
        addr->fields[i] = (char *)malloc(len);
        if (!addr->fields[i]) {
            // clean up anything we already allocated
            for (int j = 0; j < i; j++) free(addr->fields[j]);
            free(addr);
            return NULL;
        }
        memcpy(addr->fields[i], src, len);
    }

    return addr;
}

/*
 * Build an address dictionary 
*/
void buildDictionary(FILE *f, list_t *dictionary) {
    address_t *addr;
    
    // Read addresses one by one using data_read
    while ((addr = data_read(f)) != NULL) {
        insert_record(dictionary, addr);
    }
}

/*
 * Prints out the matches from each key to the output file. 
 */ 
void output_results(list_t *dict, FILE *output_file) {   
    char line[MAX_LINE_LENGTH]; // To hold the line from stdin
    assert(dict);

    // Process queries from stdin until EOF
    while (fgets(line, sizeof(line), stdin)) { // getting line from stdin 

        chomp(line); // Removes the newline character 
        fprintf(output_file, "%s\n", line); // prints the line to output file 

        // Array to store comparison counts
        int comparisons[3] = {0, 0, 0};
        int comps = 0;

        // Searches the dict for the matching records and then stores them in a linked list
        list_t *matches = search_list(dict, line, &comps, comparisons, address_get_key);

        // Prints to the output file the address from each node
        for(node_t *cur = matches->head; cur != NULL; cur = cur->next){
            address_print_file(output_file, cur->data);
        }

        // Print comparison results to stdout
        printf("%s --> %d records found - comparisons: b%d n%d s%d\n",
                line, comps, comparisons[0], comparisons[1], comparisons[2]);
                
        free_list(matches, NULL);
    }

}

/*
 * Removes the newline character
 */
void chomp(char *s) {
    if (!s) return; // returns if string is null 
    size_t n = strcspn(s, "\r\n");
    s[n] = '\0';
}

