/* dict2.c
 *
 * Created by Natty Ayano 
 * (nayano@student.unimelb.edu.au) 
 * 21/8/25
 *
 * Dictionary program for address lookup using Patricia trees.
 * Stage 2 implements Patricia tree insertion and spellchecking.
 *
 * To compile: make -B dict2
 * To run: ./dict2 2 input_file.csv output_file.txt
 * Then enter search queries on stdin, one per line.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "list.h"
#include "data.h" 
#include "patricia.h"

int main(int argc, char *argv[]) {
	//Make sure we have at least 4 tokens in the command line
	if (argc < 4) {
        fprintf(stderr, "Usage: %s stage input_file output_file\n", argv[0]);
        return EXIT_FAILURE;
    }

    int stage = atoi(argv[1]);
    char *input_filename = argv[2];
    char *output_filename = argv[3];

    if (stage != 2) {
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
    patricia_tree_t *dictionary = create_patricia_tree();
    build_patricia_dictionary(inFile, dictionary);

    // Process all queries froms stdin
    process_patricia_queries(dictionary, outFile);

    // Free all allocated memory
    free_patricia_tree(dictionary, address_free); 

    fclose(outFile);
    fclose(inFile);
    
    return EXIT_SUCCESS;
}
