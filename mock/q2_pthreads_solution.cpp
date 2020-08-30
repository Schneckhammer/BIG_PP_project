#include <cstdio>
#include <pthread.h> // Included the header (M1)

#define NUM_TASKS 1000
#define NUM_THREADS 4
#define MULTIPLIER 48271
#define MODULUS 2147483647

unsigned int startValues[NUM_TASKS];
unsigned int results[NUM_TASKS];

unsigned int nextTask = 0; // Use of shared data structure to distribute tasks (M1)
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // Creation/destruction of lock (M1)

unsigned int kernel(unsigned int startValue, unsigned int numRounds) {
    unsigned int result = startValue;
    for (unsigned int round = 0; round < numRounds; round++) {
        result = (MULTIPLIER * result) % MODULUS;
    }
    return result;
}

// Work is distributed in chunks of size one, pulled from a central data structure in each iteration, akin to a dynamic schedule (M1)
void* threadKernel(void*) { // Correct kernel signature (M1)
    while (true) {
        pthread_mutex_lock(&lock);
        if(nextTask >= NUM_TASKS) {
            pthread_mutex_unlock(&lock); // Always unlock after lock (1st part)
            break;
        }
        unsigned int task = nextTask; // Parameter read correctly (1st part)
        nextTask++;
        pthread_mutex_unlock(&lock); // Always unlock after lock (2nd part) (M1)

        results[task] = kernel(startValues[task], task); // Parameter read correctly/result stored correctly (2nd part) (M1)
    }
    return nullptr;
}

void generateProblem();

int main(int, char**) {
    generateProblem();

    // Correct creation and launch of threads (M1)
    pthread_t threads[NUM_THREADS];
    for (unsigned int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], nullptr, threadKernel, nullptr);
    }

    // Kernel runs in parallel (M1)

    // Correct joining of threads (M1)
    for (unsigned int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Correct output of the code (A3) (estimate how close student got to the correct answer).
    for (unsigned int i = 0; i < NUM_TASKS; i++) {
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
