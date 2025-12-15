/* Number of bits in a single character. */
#define BITS_PER_BYTE 8

int getBit(char *s, unsigned int bitIndex);
char *createStem(char *oldKey, unsigned int startBit, unsigned int numBits);
