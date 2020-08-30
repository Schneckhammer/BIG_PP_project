#include <pthread.h>

//Variables are initialized explicitly to the correct values for this exercise to make the code shorter

// TODO 1: global constants (so that you don't have to pass them through the create)
int max_iter = 1000;
int x_resolution = 1033;
int y_resolution = 1033;

int palette_shift = 0;
double view_x0 = -2;
double view_x1 = +2;
double view_y0 = -2;
double view_y1 = +2;

double x_stepsize = (view_x1 - view_x0) / (1.0 * x_resolution);
double y_stepsize = (view_y1 - view_y0) / (1.0 * y_resolution);

// TODO 2: num_threads
int num_threads = 16;
// TODO 3: shared iterator
int pointsInSetCount = 0;
// TODO 4: mutexes (for shared iterator and other stuff)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 


// TODO 5: rewrite the work function as a thread kernel
void* kernel (void* arg){

    // TODO 6: get rank as a parameter (to determine the chunk)
    // Note: Chunk can also be determined dynamically through shared variables `chunkstart, chunkend`
    int id = *((int*) arg);

    for (int i = id*(y_resolution/num_threads); i < id*(y_resolution/num_threads) + (y_resolution/num_threads); i++) {
        for (int j = 0; j < x_resolution; j++) {

            do_stuff();

            if (k == max_iter) {
                pthread_mutex_lock(&mutex);
                pointsInSetCount++;
                pthread_mutex_unlock(&mutex);

            }
        }
    }

    return NULL;
}


int main(int argc, char **argv) {

    getProblemFromInput(&power);

    // TODO 7: allocate threads (can be done globally too)
    pthread_t *threads = (pthread_t*) malloc (num_threads *sizeof(pthread_t));

    // TODO 8: arguments that will be passed for each thread (rank)
    int thread_args[num_threads];

    // TODO 9: create threads sequentially
    for (int i = 0; i < num_threads; ++i) {
        // TODO 10: save parameter i (rank) separately for each thread, otherwise the value may be overwritten before the thread reads it
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, kernel, &thread_args[i]);
    }

    // TODO 11: join
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    outputSolution(pointsInSetCount);

    free(threads);
    return 0;
}