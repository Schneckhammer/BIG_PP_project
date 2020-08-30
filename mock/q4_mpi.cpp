#include <climits>
#include <cstdio>

#define NUM_BLOCKS 1024

typedef unsigned long long Hash;

struct Block {
    Hash previousBlockHash;
    unsigned char data[1024];
};

Block blockchain[NUM_BLOCKS] = {};

void fillBlockWithData(unsigned char *block);

Hash hashBlock(Block &previousBlock);

int main(int argc, char **argv) {
    // blockchain[0] is just a bunch of zeros. We start with hashing and data in block 1

    for (int i = 1; i < NUM_BLOCKS; i++) {
        fillBlockWithData(blockchain[i].data);
        blockchain[i].previousBlockHash = hashBlock(blockchain[i - 1]);
    }
    printf("Last block's previous block signature: 0x%.16llx\n", blockchain[NUM_BLOCKS - 1].previousBlockHash);
}

// How the data is generated is not relevant to this exercise.
void fillBlockWithData(unsigned char *blockData) {
    for (unsigned long i = 0; i < sizeof(Block::data); i++) {
        blockData[i] = i;
    }
}

// The implementation of the hash function is not relevant to solving this exercise.
Hash hashBlock(Block &previousBlock) {
    unsigned char *data = previousBlock.data;

    Hash result = 0x341cc4f4a5d86cee;

    for (unsigned int i = 0; i < sizeof(Block::data); i++) {
        result ^= data[i];

        const unsigned int mask = (CHAR_BIT * sizeof(result) - 1);
        unsigned int c = 7;
        c &= mask;
        result = (result << c) | (result >> ((-c) & mask));
    }

    return result;
}
