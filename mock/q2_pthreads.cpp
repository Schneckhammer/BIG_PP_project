#include <cstdio>

#define NUM_TASKS 1000
#define NUM_THREADS 4
#define MULTIPLIER 48271
#define MODULUS 2147483647 // 2^31 -1

unsigned int startValues[NUM_TASKS];
unsigned int results[NUM_TASKS];

unsigned int kernel(unsigned int startValue, unsigned int numRounds) {
    unsigned int result = startValue;
    for (unsigned int round = 0; round < numRounds; round++) {
        result = (MULTIPLIER * result) % MODULUS;
    }
    return result;
}

void generateProblem();

int main(int, char **) {
    generateProblem();

    for (unsigned int i = 0; i < NUM_TASKS; i++) {
        results[i] = kernel(startValues[i], i);
        printf("Result %i: %i\n", i, results[i]);
    }
}

// These are provided just for your convenience
// Your code should run correctly no matter the start values.
void generateProblem() {
    for (int i = 0; i < NUM_TASKS; ++i) {
        startValues[i] = i;
    }
}
