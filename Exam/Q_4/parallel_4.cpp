#include <cstdio>
#include <unistd.h>
#include <cmath>
#include <thread>
#include <pthread.h>

// Place any includes you might need before this patch
#include "pthread_patch.h"


// Sleep Utility
#define DO_SLEEP(x) std::this_thread::sleep_for(std::chrono::milliseconds(x));
#define NUM_ITERATIONS 10
#define NUM_THREADS 3
pthread_t threads[NUM_THREADS];
int NUM_PROBLEMS = 100;
int value;
pthread_barrier_t barriers[2];

int inputs[NUM_ITERATIONS*3];
int outputs[NUM_ITERATIONS*3];

void* stage1(void* arg) {
	for (int i = 0; i < NUM_ITERATIONS; i++){
		inputs[i] = value++;
		printf("Stage 1 %i\n", inputs[i]);
		DO_SLEEP(100);
		inputs[i] = inputs[i]*inputs[i];
		pthread_barrier_wait(barriers);
	}
    //return in * in;
	return NULL;
}

void* stage2(void* arg) {
	for (int i = 0; i < NUM_ITERATIONS; i++){
		pthread_barrier_wait(barriers);
		printf("Stage 2 %i\n", inputs[i]);
		DO_SLEEP(400);
		inputs[i] = inputs[i]+inputs[i];
		pthread_barrier_wait(barriers+1);
	}
	return NULL;
}

void* stage3(void* arg) {
	for (int i = 0; i < NUM_ITERATIONS; i++){
		pthread_barrier_wait(barriers+1);
		printf("Stage 3 %i\n", inputs[i]);
		DO_SLEEP(200);
		inputs[i] = sqrt(inputs[i]);
		//in = sqrt(in);
		printf("Final value %i\n", inputs[i]);
	}
	return NULL;
}

int main(int, char**) {
	pthread_barrier_init(&barriers[0], NULL, 2);
	pthread_barrier_init(&barriers[1], NULL, 2);
	pthread_create(&threads[0], NULL, stage1, NULL);
	pthread_create(&threads[1], NULL, stage2, NULL);
	pthread_create(&threads[2], NULL, stage3, NULL);

	for (int i = 0; i < NUM_THREADS; i++){
		pthread_join(threads[i], NULL);
	}
	pthread_barrier_destroy(&barriers[0]);
	pthread_barrier_destroy(&barriers[1]);
    return 0;
}
