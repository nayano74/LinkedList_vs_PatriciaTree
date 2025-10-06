/* bit.h
 *
 * Header file for bit-level string comparison operations.
 * Provides functions to extract individual bits from strings and count
 * bit-level comparisons until the first mismatch for performance analysis
 */

/* Number of bits in a single character. */
#define BITS_PER_BYTE 8

int getBit(const char *s, unsigned int bitIndex);

int count_bit_comparisons(const char *key1, const char *key2);
