#include <stdio.h>
#include <mpi.h>

#define TAG_WANT_WORK 1
#define TAG_CHUNK_ID 2

#define NUM_TASKS 1000
#define MULTIPLIER 48271
#define MODULUS 2147483647 // 2^31 -1
#define CHUNK_SIZE 2

unsigned int startValues[NUM_TASKS];

unsigned int kernel(unsigned int startValue, unsigned int numRounds) {
    unsigned int result = startValue;
    for (unsigned int round = 0; round < numRounds; round++) {
        result = (MULTIPLIER * result) % MODULUS;
    }
    return result;
}

void generateProblem();

void manager() {
    // Do work
    for (int idx = 0; idx < NUM_TASKS; idx+=CHUNK_SIZE) {
        int who;
        MPI_Recv(&who, 1, MPI_INT, MPI_ANY_SOURCE, TAG_WANT_WORK,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&idx, 1, MPI_INT, who, TAG_CHUNK_ID, MPI_COMM_WORLD);
    }

    // Shutdown
    int size;
    int stop = -1;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    for (int left = size-1; left > 0; left--) {
        int who;
        MPI_Recv(&who, 1, MPI_INT, MPI_ANY_SOURCE, TAG_WANT_WORK,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&stop, 1, MPI_INT, who, TAG_CHUNK_ID, MPI_COMM_WORLD);
    }
}

void worker(int rank) {
    generateProblem();

    while (1) {
        int idx;
        MPI_Send(&rank, 1, MPI_INT, 0, TAG_WANT_WORK, MPI_COMM_WORLD);
        MPI_Recv(&idx, 1, MPI_INT, 0, TAG_CHUNK_ID, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        if (idx == -1) break;
        for (int i = 0; i < CHUNK_SIZE; i++) {
            printf("Result %i: %i\n", idx+i, kernel(startValues[idx+i], idx+i));
        }
    }
}

int main(void) {
    MPI_Init(NULL, NULL);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        manager();
    } else {
        worker(rank);
    };

    MPI_Finalize();
}

// These are provided just for your convenience
// Your code should run correctly no matter the start values.
void generateProblem() {
    for (int i = 0; i < NUM_TASKS; ++i) {
        startValues[i] = i;
    }
}
