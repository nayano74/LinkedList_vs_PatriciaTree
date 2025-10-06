/* bit.c
 *
 * Implementation of bit-level string operations for performance analysis.
 * Enables extraction of individual bits from strings and counting of
 * bit comparisons conducted until first mismatch, treating shorter strings
 * as zero-padded on the right.
 */

#include "bit.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

/*
 * Extracts a single bit from a string at the specified bit index
*/
int getBit(const char *s, unsigned int bitIndex){
    assert(s && bitIndex >=0);
    unsigned int byte = bitIndex / BITS_PER_BYTE;
    unsigned int indexFromLeft = bitIndex % BITS_PER_BYTE;
    /* 
        Since we split from the highest order bit first, the bit we are interested
        will be the highest order bit, rather than a bit that occurs at the end of the
        number. 
    */
    unsigned int offset = (BITS_PER_BYTE - (indexFromLeft) - 1) % BITS_PER_BYTE;
    unsigned char byteOfInterest = s[byte];
    unsigned int offsetMask = (1 << offset);
    unsigned int maskedByte = (byteOfInterest & offsetMask);
    /*
        The masked byte will still have the bit in its original position, to return
        either 0 or 1, we need to move the bit to the lowest order bit in the number.
    */
    unsigned int bitOnly = maskedByte >> offset;
    return bitOnly;
}

/*
 * Compares two strings bit by bit and counts comparisons until first mismatch
*/
int count_bit_comparisons(const char *key_1, const char *key_2) {
    int bit_count = 0;
    int len_1 = strlen(key_1) + 1; // Add 1 for null byte
    int len_2 = strlen(key_2) + 1;
    int max_len;

    // max_len is whichever string is longer
    if (len_1 > len_2) {
        max_len = len_1;
    } else {
        max_len = len_2;
    }

    // Compare bit by bit
    for (int bit_index = 0; bit_index < max_len * BITS_PER_BYTE; bit_index++) {
        bit_count++;

        // Get individual bits using the provided getBit function
        int bit_1;
        if (bit_index < len_1 * BITS_PER_BYTE) {
            bit_1 = getBit(key_1, bit_index);
        } else {
            bit_1 = 0;
        }

        int bit_2;
        if (bit_index < len_2 * BITS_PER_BYTE) {
            bit_2 = getBit(key_2, bit_index);
        } else {
            bit_2 = 0;
        }

        // If bits differ, stop here
        if (bit_1 != bit_2) {
            return bit_count;
        }
    }

    // If we arrive here, strings are identical
    return bit_count;

}
