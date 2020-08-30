#include <pthread.h>

#include <cstdio>
#include <unistd.h>
#include <cmath>
#include <thread>

// Place any includes you might need before this patch
#include "pthread_patch.h"



// Sleep Utility
#define DO_SLEEP(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));
#define NUM_ITERATIONS 10
int value;

int in_s2;
int in_s3;
int out_s1;
int out_s2;
bool s1_written;
bool s2_written;
bool s2_read;
bool s3_read;

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void* stage1(void*) {
    int in_s1 = value++;
    printf("Stage 1 %i\n", in_s1);
    DO_SLEEP(200);
    out_s1 = in_s1 * in_s1;
}

void* stage2(void*) {
    printf("Stage 2 %i\n", in_s2);
    DO_SLEEP(400);
    out_s2 = in_s2 + in_s2;
}

void* stage3(void*) {
    printf("Stage 3 %i\n", in_s3);
    DO_SLEEP(100);
    in_s3 = sqrt(in_s3);
    printf("Final value %i\n", in_s3);
}

int main(int, char**) {


    pthread_t threads[3];

    for (int i = 0; i < NUM_ITERATIONS; i++) {
        pthread_create(&threads[0], nullptr, stage1, nullptr);
        pthread_join(threads[0], nullptr);
        pthread_create(&threads[1], nullptr, stage2, nullptr);
        pthread_join(threads[1], nullptr);
        pthread_create(&threads[2], nullptr, stage3, nullptr);
        pthread_join(threads[2], nullptr);
    }
    return 0;
}
