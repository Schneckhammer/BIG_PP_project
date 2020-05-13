#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <complex>
#include <pthread.h>

#include "mandelbrot_set.h"

#define CHANNELS 3
// Returns the one dimensional index into our pseudo 3D array
#define OFFSET(y, x, c) (y * x_resolution * CHANNELS + x * CHANNELS + c)


const int NUM_THREADS = 32;
pthread_t threads[NUM_THREADS];
pthread_mutex_t incrementCount;
pthread_mutex_t getTask;
int curr_x_pixel = 0;  // new thread will start working from this pixel
int curr_y_pixel = 0;  // new thread will start working from this pixel
const int interval_x = 10;
const int interval_y = 10;


int pointsInSetCount = 0;

double power = 1.0;
int max_iter = 50; // sizeof(colors);
int x_resolution = 256;
int y_resolution = 256;
double view_x0 = -2;
double view_x1 = +2;
double view_y0 = -2;
double view_y1 = +2;

double x_stepsize;
double y_stepsize;



void* draw_parallel(void* args){
    using namespace std::complex_literals;

    double y;
    double x;

    int k;
    int first_x_pixel, first_y_pixel;
    int last_x_pixel, last_y_pixel;

    std::complex<double> Z;
    std::complex<double> Z_old;
    std::complex<double> C;

    while(true){
        // Mutex to get a task assigned
        pthread_mutex_lock(&getTask);
        first_x_pixel = curr_x_pixel; first_y_pixel = curr_y_pixel;
        if (first_x_pixel > x_resolution || first_y_pixel > y_resolution){
            break;
        }
        last_x_pixel = curr_x_pixel + interval_x;  // exclusively
        last_y_pixel = curr_y_pixel + interval_y;  // exclusively
        curr_x_pixel = last_x_pixel;
        // curr_y_pixel isn't changed
        if (last_x_pixel >= x_resolution){
            last_x_pixel = x_resolution;            // don't go over the border
            curr_x_pixel = 0;                           // step to the next line
            curr_y_pixel = curr_y_pixel + interval_y;   // step to the next line
        }
        if (last_y_pixel >= y_resolution){
            last_y_pixel = y_resolution;            // don't go over the border
        }
        pthread_mutex_unlock(&getTask);

        // For each pixel in the window, compute it's set count
        for (int i = first_y_pixel; i < last_y_pixel; i++) {
            for (int j = first_x_pixel; j < last_x_pixel; j++) {
                y = view_y1 - i * y_stepsize;
                x = view_x0 + j * x_stepsize;
//                if (i>1030 || j > 1030){
//                    fprintf(stderr, "(%d, %d)  ", i, j);
//                }

                Z = 0.0 + 0.0i;
                C = x + y * 1.0i;
                k = 0;

                do {
                    if (k % 30 == 0) {
                        Z_old = Z;
                    }
                    Z = std::pow(Z, power) + C;
                    k++;
                } while (std::abs(Z) < 2 && k < max_iter && Z_old != Z);

                if (k == max_iter || Z_old == Z) {
//                    fprintf(stderr, "(%d, %d)  ", max_iter, k);
                    pthread_mutex_lock(&incrementCount);
                    pointsInSetCount++;
                    pthread_mutex_unlock(&incrementCount);
                }
            }
        }
    }
    return NULL;
}



int mandelbrot_draw() {
    int result_code;

    for (int index = 0; index < NUM_THREADS; ++index) {
        result_code = pthread_create(&threads[index], NULL, draw_parallel, NULL);
    }
    for (int index = 0; index < NUM_THREADS; ++index) {
        result_code = pthread_join(threads[index], NULL);
    }
    return 0;
}


int main(int argc, char **argv) {
    struct timespec begin, end;


    int palette_shift = 10;
    char file_name[256] = "mandelbrot.ppm";
    int no_output = 0;

    // This option parsing is not very interesting.
    int c;
    while ((c = getopt(argc, argv, "t:p:i:r:v:n:f:")) != -1) {
        switch (c) {
            case 'p':
                if (sscanf(optarg, "%d", &palette_shift) != 1)
                    goto error;
                break;

            case 'i':
                if (sscanf(optarg, "%d", &max_iter) != 1)
                    goto error;
                break;

            case 'r':
                if (sscanf(optarg, "%dx%d", &x_resolution, &y_resolution) != 2)
                    goto error;
                break;
            case 'n':
                if (sscanf(optarg, "%d", &no_output) != 1)
                    goto error;
                break;
            case 'f':
                strncpy(file_name, optarg, sizeof(file_name) - 1);
                break;

            case '?':
            error:
                fprintf(stderr,
                        "Usage:\n"
                        "-i \t maximum number of iterations per pixel\n"
                        "-r \t image resolution to be computed\n"
                        "-p \t shift palette to change colors\n"
                        "-f \t output file name\n"
                        "-n \t no output(default: 0)\n"
                        "\n"
                        "Example:\n"
                        "%s -t 1 -r 720x480 -i 5000 -f mandelbrot.ppm\n",
                        argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }

    FILE *file = nullptr;
    if (!no_output) {
        if ((file = fopen(file_name, "w")) == NULL) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
    }

    // This is a pseudo 3d (1d storage) array that can be accessed using the OFFSET Macro.
    auto *image = static_cast<unsigned char *>(malloc(x_resolution * y_resolution * sizeof(unsigned char[3])));


    // Calculate the step size.
    x_stepsize = (view_x1 - view_x0) / (1.0 * x_resolution);
    y_stepsize = (view_y1 - view_y0) / (1.0 * y_resolution);

    if(x_stepsize <= 0 || y_stepsize <= 0) {
        printf("Please specify a valid range.\n");
        exit(-1);
    }

    // Don't modify anything that prints to stdout.
    // The output on standard out needs to be the same as in the sequential implementation.
    fprintf(stderr,
            "Following settings are used for computation:\n"
            "Max. iterations: %d\n"
            "Resolution: %dx%d\n"
            "View frame: [%lf,%lf]x[%lf,%lf]\n"
            "Stepsize x = %lf y = %lf\n", max_iter, x_resolution,
            y_resolution, view_x0, view_x1, view_y0, view_y1, x_stepsize,
            y_stepsize);

    if (!no_output)
        fprintf(stderr, "Output file: %s\n", file_name);
    else
        fprintf(stderr, "No output will be written\n");

    // Free time
    getProblemFromInput(&power);


    clock_gettime(CLOCK_MONOTONIC, &begin);
    // compute mandelbrot
    mandelbrot_draw();
    int numSamplesInSet = pointsInSetCount;
    clock_gettime(CLOCK_MONOTONIC, &end);
    outputSolution(numSamplesInSet);

    // Free time

    if (!no_output) {
        if (fprintf(file, "P6\n%d %d\n%d\n", x_resolution, y_resolution, 255) < 0) {
            std::perror("fprintf");
            exit(EXIT_FAILURE);
        }
        size_t bytes_written = fwrite(image, 1,
                                      x_resolution * y_resolution * sizeof(unsigned char[3]), file);
        if (bytes_written < x_resolution * y_resolution * sizeof(char[3])) {
            std::perror("fwrite");
            exit(EXIT_FAILURE);
        }
//        for (int i = 0; i < y_resolution; ++i) {
//            for (int j = 0; j < x_resolution; ++j) {
//                fprintf(file, "%i %i %i ", image[OFFSET(i, j, 0)], image[OFFSET(i, j, 1)], image[OFFSET(i, j, 2)]);
//            }
//            fprintf(file, "\n");
//        }
        fclose(file);
    }

    // This is just here for convenience, it isn't actually used to measure your runtime
    fprintf(stderr, "\n\nTime: %.5f seconds\n", ((double) end.tv_sec + 1.0e-9 * end.tv_nsec) -
                                                ((double) begin.tv_sec + 1.0e-9 * begin.tv_nsec));

    free(image);
    return 0;
}
