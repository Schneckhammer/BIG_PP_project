#include <climits>
#include <cstdio>
#include <mpi.h> // Included <mpi.h> (M1)

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
    MPI_Init(&argc, &argv); // MPI initialized and finalized correctly (1st part)
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Rank and size determined correctly (M1)

    MPI_Request recvRequest = MPI_REQUEST_NULL, sendRequest = MPI_REQUEST_NULL;
    // blockchain[0] is just a bunch of zeros. We start with hashing and data in block 1

    // Correct adjustment of loop counters (in any way that works) (M1)
    for (int i = rank; i < NUM_BLOCKS; i += size) {
        // Don't receive on first iteration (M1)
        if (i != 0) {
            // Receive previous block from lower neighbor correctly (M1)
            MPI_Irecv(&blockchain[i - 1], sizeof(Block), MPI_UNSIGNED_CHAR,
                      MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &recvRequest);
        }

        fillBlockWithData(blockchain[i].data);
        // Complete read request at correct location (M1)
        MPI_Wait(&recvRequest, MPI_STATUS_IGNORE);
        blockchain[i].previousBlockHash = hashBlock(blockchain[i - 1]);

        // Don't send on last iteration (M1)
        if (i != NUM_BLOCKS - 1) {
            // Complete write request at correct location (M1)
            MPI_Wait(&sendRequest, MPI_STATUS_IGNORE);
            // Send current block to neighbor correctly (M1)
            MPI_Send(&blockchain[i], sizeof(Block), MPI_UNSIGNED_CHAR,
                     (rank + 1) % size, 0, MPI_COMM_WORLD);
        } else {
            // Correct output of the code (A3) (estimate how close student got to the correct answer).
            printf("Last block's previous block signature: 0x%.16llx\n", blockchain[NUM_BLOCKS - 1].previousBlockHash);
        }
    }

    // Don't finalize before all requests complete (M1)
    MPI_Wait(&sendRequest, MPI_STATUS_IGNORE);
    MPI_Finalize(); // MPI initialized and finalized correctly (2nd part) (M1)
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
