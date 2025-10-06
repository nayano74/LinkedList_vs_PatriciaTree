/* main.c
 *
 * Created by Armaan Singh Sidhu and Natty Ayano 
 * (ArmaanSingh.Sidhu@student.unimelb.edu.au and nayano@student.unimelb.edu.au) 
 * 21/8/25
 *
 * Dictionary program for address lookup using linked lists.
 * Stage 1 implements basic key lookup functionality using EZI_ADD field.
 *
 * To compile: make -B dict1
 * To run: ./dict1 1 input_file.csv output_file.txt
 * Then enter search queries on stdin, one per line.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "list.h"
#include "data.h" 

int main(int argc, char *argv[]) {
	//Make sure we have at least 4 tokens in the command line
	if (argc < 4) {
        fprintf(stderr, "Usage: %s stage input_file output_file\n", argv[0]);
        return EXIT_FAILURE;
    }

    int stage = atoi(argv[1]);
    char *input_filename = argv[2];
    char *output_filename = argv[3];

    if (stage != 1) {
        fprintf(stderr, "Only stage 1 is to be implemented\n");
        return EXIT_FAILURE;
    }

    // Open files
	FILE *inFile = fopen(input_filename, "r");
    if (!inFile) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    FILE *outFile = fopen(output_filename, "w");
    if (!outFile) {
        perror("Error opening output file");
        fclose(inFile);
        return EXIT_FAILURE;
    }

    // Create dictionary and build it
    list_t *dictionary = create_list();
    buildDictionary(inFile, dictionary);
    
    // address_print_file(outFile, dictionary->head->data);
    output_results(dictionary, outFile);

    fclose(outFile);
    fclose(inFile);

    free_list(dictionary, address_free);

    return EXIT_SUCCESS;
}
