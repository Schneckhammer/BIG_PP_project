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

int stage1() {
    int in = value++;
    printf("Stage 1 %i\n", in);
    DO_SLEEP(200);
    return in * in;
}

int stage2(int in) {
    printf("Stage 2 %i\n", in);
    DO_SLEEP(400);
    return in + in;
}

void stage3(int in) {
    printf("Stage 3 %i\n", in);
    DO_SLEEP(100);
    in = sqrt(in);
    printf("Final value %i\n", in);
}

int main(int, char**) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        int tmp = stage1();
        tmp = stage2(tmp);
        stage3(tmp);
    }
    return 0;
}
