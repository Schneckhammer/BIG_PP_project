#include <cstdio>
#include <cstdlib>

#define NUM_THREADS 30
#define GRID_SIZE 5
#define NUM_PROBLEMS 6

int valuesArray[GRID_SIZE][GRID_SIZE];

void fillArray(int (&array)[GRID_SIZE][GRID_SIZE]);

int main(int, char **) {
    int x, y, operationResult;

    // Note that this function is not thread safe
    fillArray(valuesArray);

    for (int problem = 0; problem < NUM_PROBLEMS; ++problem) {
        operationResult = 1;

        for (x = 0; x < GRID_SIZE; x++) {
            for (y = 0; y < GRID_SIZE; y++) {
                int localResult = valuesArray[x][y] * problem;

                if (valuesArray[x][y] < 0) {
                    operationResult = -1;
                    goto finish;
                }

                operationResult += localResult;
            }
        }

        finish:
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
