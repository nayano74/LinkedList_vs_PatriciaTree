/* patricia.c
 *
*/


#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "patricia.h"


/* -- Prototypes for statically defined functions --*/
static void patricia_insert(patricia_tree_t *tree, const char *key, void *data);
static patricia_node_t *create_patricia_node(char *prefix, unsigned int prefixBits);
static unsigned int compare_and_count(const char *key, unsigned int key_start_bit,
                                      const char *prefix, unsigned int prefix_bits);
static list_t *patricia_search_exact(patricia_tree_t *tree, const char *key, search_results_t *results);
static const char *get_key_from_data_list(list_t *data_list);
static void collect_keys_in_subtree(patricia_node_t *node, list_t *key_list);
static int min(int a, int b, int c);
static int editDistance(char *str1, char *str2, int n, int m);

/* -- Provided helper functions -- */

/* Allocates new memory to hold the numBits specified and fills the allocated
    memory with the numBits specified starting from the startBit of the oldKey
    array of bytes. */
char *createStem(char *oldKey, unsigned int startBit, unsigned int numBits) {
    assert(numBits > 0 && startBit >= 0 && oldKey);
    int extraBytes = 0;
    if((numBits % BITS_PER_BYTE) > 0){
        extraBytes = 1;
    }
    unsigned int totalBytes = (numBits / BITS_PER_BYTE) + extraBytes;
    char *newStem = malloc(sizeof(char) * totalBytes);
    assert(newStem);
    for(unsigned int i = 0; i < totalBytes; i++){
        newStem[i] = 0;
    }
    for(unsigned int i = 0; i < numBits; i++){
        unsigned int indexFromLeft = i % BITS_PER_BYTE;
        unsigned int offset = (BITS_PER_BYTE - indexFromLeft - 1) % BITS_PER_BYTE;
        unsigned int bitMaskForPosition = 1 << offset;
        unsigned int bitValueAtPosition = getBit(oldKey, startBit + i);
        unsigned int byteInNewStem = i / BITS_PER_BYTE;
        newStem[byteInNewStem] |= bitMaskForPosition * bitValueAtPosition;
    }
    return newStem;
}

/* Returns min of 3 integers 
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
int min(int a, int b, int c) {
    if (a < b) {
        if(a < c) {
            return a;
        } else {
            return c;
        }
    } else {
        if(b < c) {
            return b;
        } else {
            return c;
        }
    }
}

/* Returns the edit distance of two strings
    reference: https://www.geeksforgeeks.org/edit-distance-in-c/ */
int editDistance(char *str1, char *str2, int n, int m){
    assert(m >= 0 && n >= 0 && (str1 || m == 0) && (str2 || n == 0));
    // Declare a 2D array to store the dynamic programming
    // table
    int dp[n + 1][m + 1];

    // Initialize the dp table
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            // If the first string is empty, the only option
            // is to insert all characters of the second
            // string
            if (i == 0) {
                dp[i][j] = j;
            }
            // If the second string is empty, the only
            // option is to remove all characters of the
            // first string
            else if (j == 0) {
                dp[i][j] = i;
            }
            // If the last characters are the same, no
            // modification is necessary to the string.
            else if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = min(1 + dp[i - 1][j], 1 + dp[i][j - 1],
                    dp[i - 1][j - 1]);
            }
            // If the last characters are different,
            // consider all three operations and find the
            // minimum
            else {
                dp[i][j] = 1 + min(dp[i - 1][j], dp[i][j - 1],
                    dp[i - 1][j - 1]);
            }
        }
    }

    // Return the result from the dynamic programming table
    return dp[n][m];
}

/* -- Core Patricia Tree Implementation--*/

/**
 * Creates and initialises a new, empty Patricia tree
 * Returns a pointer to the newly allocated tree
 */
patricia_tree_t *create_patricia_tree() {
    // Allocate memory for tree structure.
    patricia_tree_t *tree = malloc(sizeof(patricia_tree_t));
    assert(tree);

    tree->root = NULL;
    tree->num_key = 0;

    return tree;
}

/**
 * Helper function create and initialise a single Patricia tree node.
 * It allocates memory for the node and its prefix.
 *
 * prefix: A pointer to the character array containing a key or part thereof
 * prefix_bits: The number of bits in the prefix
 *
 * Returns a pointer to the newly allocated node
*/
patricia_node_t *create_patricia_node(char *prefix, unsigned int prefixBits) {
    patricia_node_t *node = malloc(sizeof(patricia_node_t));
    assert(node);

    node->prefixBits = prefixBits;

    int numBytes = (prefixBits + BITS_PER_BYTE - 1) / BITS_PER_BYTE;
    node->prefix = malloc(numBytes);
    assert(node->prefix);
    memcpy(node->prefix, prefix, numBytes);

    node->branch[0] = NULL;
    node->branch[1] = NULL;
    node->data = NULL;

    return node;
}

/**
 * Inserts a new record into the tree
 * tree: the Patricia tree into which the record is to be insert_record
 * key: the key to be insert_record
 * data: the record to which the key corresponds
 */
void patricia_insert(patricia_tree_t *tree, const char *key, void *data) {
    assert(tree && key && data);
    
    unsigned int total_key_bits = (strlen(key) + 1) * BITS_PER_BYTE;

    if (tree->root == NULL) {
        // Tree is empty
        char *stem = createStem((char *)key, 0, total_key_bits);
        patricia_node_t *newNode = create_patricia_node(stem, total_key_bits);
        newNode->data = create_list();
        insert_record(newNode->data, data);
        tree->root = newNode;
        tree->num_key++;
        free(stem);
        return;
    }

    // Tree is not empty. Begin traversal
    patricia_node_t *current = tree->root;
    patricia_node_t *parent = NULL;
    int parent_branch_bit = 0;
    unsigned int bits_matched_so_far = 0;

    while (1) {
        unsigned int matched_in_node = compare_and_count(key, bits_matched_so_far,
                                                         current->prefix, current->prefixBits);

        if (matched_in_node < current->prefixBits) {
            // --- NODE SPLIT LOGIC ---
            char *common_stem = createStem(current->prefix, 0, matched_in_node);
            patricia_node_t *new_parent = create_patricia_node(common_stem, matched_in_node);
            free(common_stem);

            // Rearrange the old current node to become a child
            unsigned int old_rem_bits = current->prefixBits - matched_in_node;
            char *old_rem_stem = createStem(current->prefix, matched_in_node, old_rem_bits);
            free(current->prefix);
            current->prefix = old_rem_stem;
            current->prefixBits = old_rem_bits;
            
            // Handle when the new key has no leftover bits
            unsigned int new_key_rem_bits = total_key_bits - (bits_matched_so_far + matched_in_node);
            if (new_key_rem_bits == 0) {
                // The new key ends exactly at the split. Its data goes in the new parent.
                new_parent->data = create_list();
                insert_record(new_parent->data, data);
                tree->num_key++; // It's a new, distinct key.
            } else {
                // The new key has a remainder. Create a new child for it
                char *new_rem_stem = createStem((char*)key, bits_matched_so_far + matched_in_node, new_key_rem_bits);
                patricia_node_t *new_child = create_patricia_node(new_rem_stem, new_key_rem_bits);
                free(new_rem_stem);
                new_child->data = create_list();
                insert_record(new_child->data, data);
                tree->num_key++;
                int new_key_next_bit = getBit((char *)key, bits_matched_so_far + matched_in_node);
                new_parent->branch[new_key_next_bit] = new_child;
            }
            
            int old_node_next_bit = getBit(current->prefix, 0); // first bit of its new remainder
            new_parent->branch[old_node_next_bit] = current;
            
            if (parent == NULL) { 
                tree->root = new_parent; 
            } else { 
                parent->branch[parent_branch_bit] = new_parent; 
            }
            return;
        }

        bits_matched_so_far += current->prefixBits;

        if (bits_matched_so_far >= total_key_bits) {
            // Exact match of an existing key, just add data
            if (current->data == NULL) { 
                current->data = create_list(); 
            }
            insert_record(current->data, data);
            return;
        }

        int next_bit = getBit((char *)key, bits_matched_so_far);

        if (current->branch[next_bit] == NULL) {
            // Path ends, create a new leaf
            unsigned int rem_bits = total_key_bits - bits_matched_so_far;
            char *rem_stem = createStem((char *)key, bits_matched_so_far, rem_bits);
            patricia_node_t *new_leaf = create_patricia_node(rem_stem, rem_bits);
            free(rem_stem);
            new_leaf->data = create_list();
            insert_record(new_leaf->data, data);
            tree->num_key++;
            current->branch[next_bit] = new_leaf;
            return;
        }
        
        // Continue traversal
        parent = current;
        parent_branch_bit = next_bit;
        current = current->branch[next_bit];
    }
}

/**
 * Build patricia tree dictionary
 *
 * inFile: the dataset to be processed
 * dictionary: empty dictionary to be molded 
 */
void build_patricia_dictionary(FILE *inFile, patricia_tree_t *dictionary) {
    address_t *addr;
    
    // Read addresses one by one using data_read
    while ((addr = data_read(inFile)) != NULL) {
        // Get the key for the current address record
        const char *key = address_get_key(addr);
        if (key && strlen(key) > 0) {
            // Insert the record into the Patricia tree using the key
            patricia_insert(dictionary, key, addr);
        } else {
            // If a key is empty or invalid, free the record to avoid leaks
            address_free(addr);
        }
    }
}

/**
 * Compares the bits of a key against a prefix
 *
 * key: The full search key
 * key_start_bit: The bit index in the key where comparison should begin
 * prefix: The node's prefix to compare against
 * prefix_bits: The number of bits in the node's prefix
 *
 * Returns: The number of bits that matched sequentially from the start
 */
static unsigned int compare_and_count(const char *key, unsigned int key_start_bit,
                                      const char *prefix, unsigned int prefix_bits) {
    unsigned int matched_count = 0;
    unsigned int total_key_bits = (strlen(key) + 1) * BITS_PER_BYTE;

    for (unsigned int i = 0; i < prefix_bits; i++) {
        // Can't read past the end of the key
        if (key_start_bit + i >= total_key_bits) {
            break;
        }

        int key_bit = getBit((char *)key, key_start_bit + i);
        int prefix_bit = getBit((char *)prefix, i);

        if (key_bit != prefix_bit) {
            break; // Mismatch found
        }
        matched_count++;
    }
    return matched_count;
}

/**
 * Searches the Patricia tree for an exact key match
 *
 * tree: The Patricia tree to be searched
 * key: The exact sought after key string
 *
 * Returns: A new list_t containing pointers to all corresponding data records
 *          The caller is responsible for freeing this list
 *          Returns an empty list if no match is found
 */
list_t *patricia_search_exact(patricia_tree_t *tree, const char *key, search_results_t *results) {
    assert(tree && key);

    // List to hold results
    list_t *product = create_list();

    // Case in which tree is empty
    if (tree->root == NULL) {
        return product; // Return the empty list
    }

    // Prepare and commence traversal
    patricia_node_t *current = tree->root;
    unsigned int total_key_bits = (strlen(key) + 1) * BITS_PER_BYTE;
    unsigned int bits_matched_so_far = 0;

    while (current != NULL) {
        // Accessing a node
        if (results){
            results->node_comps++;
        }

        // Compare the key with the current node's prefix
        unsigned int matched_in_node = 0;
        for (unsigned int i = 0; i < current->prefixBits; i++) {
            if (bits_matched_so_far + i >= total_key_bits) break;
            
            int key_bit = getBit((char *)key, bits_matched_so_far + i);
            int prefix_bit = getBit((char *)current->prefix, i);

            // One bit is being compared
            if (results) results->bit_comps++;
            
            if (key_bit != prefix_bit) {
                break;
            }
            matched_in_node++;
        }
        
        // If it didn't match the whole prefix, the exact key cannot be in the tree
        if (matched_in_node < current->prefixBits) {
            break; // Exit lopp. No match found
        }

        // Prefix matched. Update
        bits_matched_so_far += current->prefixBits;

        // Check if exactly the right amount of bits was processed
        if (bits_matched_so_far == total_key_bits) {
            // Match. Check if current node stores data
            if (current->data != NULL) {
                // Copy all records into the results list
                const char *node_key = get_key_from_data_list(current->data);
                if (node_key) {
                    if (results) {
                        results->string_comps++;
                    }
                    if (strcmp(key, node_key) == 0) {
                        for (node_t *cur_data = current->data->head; cur_data != NULL; cur_data 
                        = cur_data->next) {
                        insert_record(product, cur_data->data);
                        }
                    }
                }
            }
            // Empty or full, regardless, the search is complete
            break;
        }

        // Key is shorter than the path thus far, no match
        if (total_key_bits < bits_matched_so_far) {
            break; // Exit loop, no match
        }

        // Key is longer, continue down branch
        int next_bit = getBit((char *)key, bits_matched_so_far);
        current = current->branch[next_bit]; // Move to the next node
    }

    return product;
}

/**
 * Searches the Patricia tree, finding an exact match or the closest spelling match
 *
 * tree: The Patricia tree in which to search
 * key: The key string to find
 *
 * Returns: A new list struct containing pointers to all matching data records
 *          Returns an empty list if no exact or similar match can be found.
 */
list_t *patricia_search_spell(patricia_tree_t *tree, const char *key, search_results_t *results) {
    assert(tree && key);

    // Attempt exact search first
    list_t *exact_matches = patricia_search_exact(tree, key, results);
    if (exact_matches->num_node > 0) {
        // Exact match found
        return exact_matches;
    }
    // No exact match found. Free empty list and proceed
    free_list(exact_matches, NULL);

    // Reset bit/node counts if exact search failed
    if (results) {
        results->bit_comps = 0;
        results->node_comps = 0;
    }

    // No exact match. Find point of failure
    if (tree->root == NULL) {
        return create_list(); // Tree is empty
    }

    patricia_node_t *current = tree->root;
    patricia_node_t *last_good_node = tree->root;
    unsigned int total_key_bits = (strlen(key) + 1) * BITS_PER_BYTE;
    unsigned int bits_matched_so_far = 0;
    
    while (current != NULL) {
        // Accessing a node
        if (results) {
            results->node_comps++;
        }

        unsigned int matched_in_node = 0;
        for (unsigned int i = 0; i < current->prefixBits; i++) {
            if (bits_matched_so_far + i >= total_key_bits) {
                break;
            }
            int key_bit = getBit((char *)key, bits_matched_so_far + i);
            int prefix_bit = getBit((char *)current->prefix, i);
            if (results) results->bit_comps++;
            if (key_bit != prefix_bit) break;
            matched_in_node++;
        }
        
        if (matched_in_node < current->prefixBits) {
            // Mismatch within the prefix. The `current` node is failure point
            last_good_node = current;
            break;
        }

        bits_matched_so_far += current->prefixBits;
        last_good_node = current; // This node was a good match

        if (bits_matched_so_far >= total_key_bits) break;

        int next_bit = getBit((char *)key, bits_matched_so_far);
        current = current->branch[next_bit];
    }

    // last_good_node is the root of the subtree with all likely candidates
    list_t *candidate_keys = create_list();
    collect_keys_in_subtree(last_good_node, candidate_keys);

    if (candidate_keys->num_node == 0) {
        // No candidates found
        free_list(candidate_keys, NULL);
        return create_list();
    }

    // Find the best match among the candidates
    char *best_match_key = NULL;
    int min_distance = -1;

    for (node_t *cur = candidate_keys->head; cur != NULL; cur = cur->next) {
        // Each call is one string comparison
        if (results) {
            results->string_comps++;
        }

        char *candidate_key = (char *)cur->data;
        int distance = editDistance((char *)key, candidate_key, strlen(key), strlen(candidate_key));
        
        if (min_distance == -1 || distance < min_distance) {
            min_distance = distance;
            best_match_key = candidate_key;
        }
        // tie-break here: if distances are equal, return the first one encountered
    }

    list_t *final_results;
    if (best_match_key) {
        // best match found
        final_results = patricia_search_exact(tree, best_match_key, NULL);
    } else {
        final_results = create_list();
    }

    // Clean up the list of candidate key strings
    free_list(candidate_keys, free);

    return final_results;
}

/**
 * Prints out the matches from each key to the output file as well as results to stdout
 * dict: the patricia tree to process
 * output_file: the file in which matches get printed
 */
void process_patricia_queries(patricia_tree_t *dict, FILE *output_file) {
    char line[MAX_LINE_LENGTH]; // MAX_LINE_LENGTH from data.h

    // Process queries from stdin until EOF
    while (fgets(line, sizeof(line), stdin)) {
        chomp(line); // Removes the newline character
        fprintf(output_file, "%s\n", line); // Print the query to the output file

        search_results_t results = {0};
        // Search the dictionary for the EXACT key
        list_t *matches = patricia_search_spell(dict, line, &results);

        // Print all matching records to the output file
        for (node_t *cur = matches->head; cur != NULL; cur = cur->next) {
            address_print_file(output_file, cur->data);
        }

        // Print comparison results to stdout
        printf("%s --> %d records found - comparisons: b%d n%d s%d\n",
                line, matches->num_node, results.bit_comps, results.node_comps,
                results.string_comps);
        
        // Search function returns a new list that must be freed
        free_list(matches, NULL);
    }
}

/**
 * Helper to get the key string from a data list
 * All records in the list should share the same key
 * Returns the key from the first record
 */
static const char *get_key_from_data_list(list_t *data_list) {
    if (data_list == NULL || data_list->head == NULL) {
        return NULL;
    }
    return address_get_key(data_list->head->data);
}

/**
 * Recursively traverses a subtree and collects all complete keys into a list.
 *
 * node: The starting node of the subtree to traverse.
 * key_list: The list where the found key strings will be stored.
 */
static void collect_keys_in_subtree(patricia_node_t *node, list_t *key_list) {
    // Base case. End of branch
    if (node == NULL) {
        return;
    }

    // Check if the current node represents a complete key.
    if (node->data != NULL) {
        // This node has data, so it's a valid key. Add it to our list.
        const char *key_string = get_key_from_data_list(node->data);
        if (key_string) {
            // Store a COPY of the key so the list owns its own memory.
            char *key_copy = malloc(strlen(key_string) + 1);
            assert(key_copy);
            strcpy(key_copy, key_string);
            insert_record(key_list, key_copy);
        }
    }

    // Recursively call for both children to explore the entire subtree.
    collect_keys_in_subtree(node->branch[0], key_list);
    collect_keys_in_subtree(node->branch[1], key_list);
}

/**
 * Recursive helper function to free a node and its children.
 * Follows a post-order traversal to safely free memory from the bottom up.
 */
static void free_patricia_node(patricia_node_t *node, void (*data_free)(void *)) {
    // Base case: If the node is NULL, there's nothing to do.
    if (node == NULL) {
        return;
    }

    // Recurse and free the children first.
    free_patricia_node(node->branch[0], data_free);
    free_patricia_node(node->branch[1], data_free);

    // Now that the children are gone, free the contents of this node
    // Free the prefix string
    free(node->prefix);

    // If there is a data list, free it and all of its contituent records
    if (node->data != NULL) {
        free_list(node->data, data_free);
    }

    // Free the patricia node struct itself.
    free(node);
}

/**
 * Frees all memory associated with a Patricia tree, including all nodes,
 * prefixes, and the data records stored within.
 *
 * tree: The tree to be freed.
 * data_free: A function pointer to a function that can free a single data record.
 */
void free_patricia_tree(patricia_tree_t *tree, void (*data_free)(void *)) {
    if (tree == NULL) {
        return;
    }

    // Free nodes first
    free_patricia_node(tree->root, data_free);

    // Free the main tree container struct last
    free(tree);
}
