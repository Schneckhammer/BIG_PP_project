#include <cstdio>
#include <cstdlib>
#include <omp.h> // Included omp.h (M1)

#define NUM_THREADS 30
#define GRID_SIZE 5
#define NUM_PROBLEMS 6

int valuesArray[GRID_SIZE][GRID_SIZE];

void fillArray(int (&array)[GRID_SIZE][GRID_SIZE]);

int main(int, char **) {
    int x, y, operationResult;

    // Note that this function is not thread safe
    fillArray(valuesArray);

    omp_set_nested(2); // Used omp_set_nested correctly (M1)

    // Used the correct number of threads (outer 5, inner 6, 5*6 = 30) (M1)
    // Outer loop parallelized correctly (M1)
    // Fixed sharing of operationResult variable by any means of privatization (M1)
    #pragma omp parallel for ordered private(operationResult) num_threads(6)
    for (int problem = 0; problem < NUM_PROBLEMS; ++problem) {
        operationResult = 1;

        bool flag = false;
        // Inner loop parallelized correctly (M1)
        // Reduced value correctly by any means (M2)
        // Bonus point: Caught that it is necessary to privatize loop counter y (B1)
        #pragma omp parallel for reduction(+: operationResult) num_threads(5) private(y)
        for (x = 0; x < GRID_SIZE; x++) {
            for (y = 0; y < GRID_SIZE; y++) {
                int localResult = valuesArray[x][y] * problem;

                if (valuesArray[x][y] < 0) {
                    // Fixed invalid exit from OpenMP structured block by any means (M1)
                    flag = true;
                }

                operationResult += localResult;
            }
        }
        if(flag) {
            operationResult = -1;
        }

        #pragma omp ordered
        printf("Solution %i: %i\n", problem, operationResult);
    }
}

// These are provided just for your convenience
// Your code should run correctly no matter the array values.
// Note that this function is not thread safe
void fillArray(int (&array)[GRID_SIZE][GRID_SIZE]) {
    srand(123);
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            array[i][j] = rand() % 32;
        }
    }
}
