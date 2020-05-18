#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <complex>
#include <pthread.h>
#include <fstream>

#include "mandelbrot_set.h"

#define CHANNELS 3
// Returns the one dimensional index into our pseudo 3D array
#define OFFSET(y, x, c) (y * x_resolution * CHANNELS + x * CHANNELS + c)

int start_for_all_x = 0;
int start_for_all_y = 0;
const int parallel_portion_y = 100;
const int parallel_portion_x = 10;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_count = PTHREAD_MUTEX_INITIALIZER;
const int NUM_THREADS = 32;
//pthread_t threads[NUM_THREADS];

bool truths[1033][1033];

int pointsInSetCount = 0;

typedef struct {
	int x_res;
	int y_res;
	int max_iter;
	double view_x0;
	double view_y1;
	double x_stepsize;
	double y_stepsize;
	double power;

} arguments;

/*
 * TODO@Students: This is your kernel. Take a look at any dependencies and decide how to parallelize it.
 */


bool check_point(double x, double y, int max_iter, double power){
	using namespace std::complex_literals;
	std::complex<double> Z;
	std::complex<double> C;

	double Zreal;
	double Zimag;
	int k;
	Z = 0.0 + 0.0i;
	C = x + y * 1.0i;

	k = 0;

	
	// Apply the Mandelbrot calculation until the absolute value >= 2 (meaning the calculation will diverge to
	// infinity) or the maximum number of iterations was reached.
	std::complex<double> Z_for_cycles = Z;
	do {
		Z = std::pow(Z, power) + C;
		if (Z == Z_for_cycles){
			k = max_iter;
			break;
		}
		Zreal = Z.real();
		Zimag = Z.imag();
		k++;
		if (k % 30 == 0)
			Z_for_cycles = Z;
	} while (Zreal*Zreal + Zimag*Zimag < 4 && k < max_iter);

	// If the maximum number of iterations was reached then this point is in the Mandelbrot set and we color it
	// black. Else, it is outside and we color it with a color that corresponds to how many iterations there
	// were before we confirmed the divergence.
	if (k == max_iter) {
		pthread_mutex_lock(&mutex_count);
		pointsInSetCount++;
		pthread_mutex_unlock(&mutex_count);
		return true;
	}
	return false;
	
}


void* parallel_part(void* args_set){
	arguments *args = (arguments*) args_set;

	int x_resolution = args->x_res;
	int y_resolution = args->y_res;
	int max_iter = args->max_iter;
	double view_x0 = args->view_x0;
	double view_y1 = args->view_y1;
	double x_stepsize = args->x_stepsize;
	double y_stepsize = args->y_stepsize;
	double power = args->power;


	while(true){
		// lock while accessing global vars
		pthread_mutex_lock(&mutex);
		// know when to stop
		if (start_for_all_y >= y_resolution && start_for_all_x + parallel_portion_x >= x_resolution ){
			pthread_mutex_unlock(&mutex);
			break;
		}
		int start_for_this_one_y = start_for_all_y;
		int start_for_this_one_x = start_for_all_x;
		start_for_all_x += parallel_portion_x;
		if (start_for_all_x >= x_resolution){
			start_for_all_y += parallel_portion_y;
			start_for_all_x = 0;
		}
		pthread_mutex_unlock(&mutex);

		int ending_x = start_for_this_one_x + parallel_portion_x;
		int ending_y = start_for_this_one_y + parallel_portion_y;

		if (ending_x > x_resolution){
			ending_x = x_resolution;
		}

		if (ending_y > y_resolution){
			ending_y = y_resolution;
		}



		double y;
		double x;



		// For each pixel in the image
		for (int i = start_for_this_one_y; i < ending_y; i++) {
			for (int j = start_for_this_one_x; j < ending_x; j++) {
				y = view_y1 - i * y_stepsize;
				x = view_x0 + j * x_stepsize;
				bool current_bool = check_point(x, y, max_iter, power);
				if (not current_bool)
					truths[i][j] = false;
				
			}

		}
	}
	return NULL;

}
void mandelbrot_draw(int x_resolution, int y_resolution, int max_iter,
                    double view_x0, double view_y1,
                    double x_stepsize, double y_stepsize,
                    double power) {
	arguments* args = new arguments;

	args->x_res = x_resolution;
	args->y_res = y_resolution;
	args->max_iter = max_iter;
	args->view_x0 = view_x0;
	args->view_y1 = view_y1;
	args->x_stepsize = x_stepsize;
	args->y_stepsize = y_stepsize;
	
	std::ofstream myfile;	
	myfile.open("training.csv", std::ios::out);
	for (int j = 0; j < 1001; j++){
		pointsInSetCount = 0;
		start_for_all_x = 0;
		start_for_all_y = 0;
		pthread_t *threads = (pthread_t*) malloc (NUM_THREADS *sizeof(pthread_t));	
		args->power = 2 + 0.001*j;
		std::cout << args->power << " ";
		
		for (int i = 0; i < NUM_THREADS; i++){
			//std::cout << "creating_thread" << std::endl;
			pthread_create(&threads[i], NULL, parallel_part, args);
			//std::assert(!return_message);
		}

		for (int i = 0; i < NUM_THREADS; i++){
			//std::cout << "creating_thread" << std::endl;
			pthread_join(threads[i], NULL);
			//std::assert(!return_message);
		}
		std::cout << pointsInSetCount << std::endl;
		//std::ofstream out("training.csv");
		//std::streambuf *coutbuf = std::cout.rdbuf();
		//std::cout.rdbuf(out.rdbuf());
		myfile << args->power << ',' << pointsInSetCount << std::endl;
		//std::cout.rdbuf(coutbuf);
		delete(threads);
	}
	myfile.close();
	delete(args);
}


int main(int argc, char **argv) {
	for (int i = 0; i < 1033; i++){
		for (int j = 0; j < 1033; j++){
			truths[i][j] = true;
		}
	}
    struct timespec begin, end;
    /*
     * TODO@Students: Decide how many threads will get you an appropriate speedup.
     */
    double power = 1.0;
    int max_iter = 50; // sizeof(colors);
    int x_resolution = 256;
    int y_resolution = 256;
    int palette_shift = 0;
    double view_x0 = -2;
    double view_x1 = +2;
    double view_y0 = -2;
    double view_y1 = +2;
    char file_name[256] = "mandelbrot.ppm";
    int no_output = 0;

    double x_stepsize;
    double y_stepsize;

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

    getProblemFromInput(&power);


    clock_gettime(CLOCK_MONOTONIC, &begin);
    // compute mandelbrot
	mandelbrot_draw(x_resolution, y_resolution, max_iter,
                                          view_x0, view_y1,
                                          x_stepsize, y_stepsize, power);
    int numSamplesInSet = pointsInSetCount;
    clock_gettime(CLOCK_MONOTONIC, &end);
    outputSolution(numSamplesInSet);

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
