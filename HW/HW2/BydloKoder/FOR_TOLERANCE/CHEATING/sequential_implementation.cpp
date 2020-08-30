#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <complex>

#include "mandelbrot_set.h"

#define CHANNELS 3
// Returns the one dimensional index into our pseudo 3D array
#define OFFSET(y, x, c) (y * x_resolution * CHANNELS + x * CHANNELS + c)

/*
 * TODO@Students: This is your kernel. Take a look at any dependencies and decide how to parallelize it.
 */

int pointsInSetCount = 0;

bool check_point(double x, double y, int max_iter, double power){
    std::complex<double> Z;
    std::complex<double> C;

    using namespace std::complex_literals;
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
		k++;
		if (k % 20 == 0)
			Z_for_cycles = Z;
	} while (std::abs(Z) < 2 && k < max_iter);

	// If the maximum number of iterations was reached then this point is in the Mandelbrot set and we color it
	// black. Else, it is outside and we color it with a color that corresponds to how many iterations there
	// were before we confirmed the divergence.
	if (k == max_iter) {
		pointsInSetCount++;
		return true;
	}

	return false;

}


int mandelbrot_draw(int x_resolution, int y_resolution, int max_iter,
                    double view_x0, double view_x1, double view_y0, double view_y1,
                    double x_stepsize, double y_stepsize,
                    int palette_shift, unsigned char *img,
                    double power, bool no_output) {

    double y;
    double x;

	int len_square = 4;

	//y_resolution = 17;
	//x_resolution = 17;

    // For each pixel in the image
    for (int i = 0; i < y_resolution-1; i += len_square) {
        for (int j = 0; j < x_resolution-1; j += len_square) {
			bool all = true;
			bool current;
			for(int l = i; l < i + len_square; l++){
				y = view_y1 - l * y_stepsize;
				x = view_x0 + j * x_stepsize;
				//std::cout << l << ' ' <<  j << std::endl;
				current = check_point(x, y, max_iter, power);
				if (!current)
					all = false; 
				y = view_y1 - l * y_stepsize;
				x = view_x0 + (j+len_square-1) * x_stepsize;
				//std::cout << l << ' ' <<  (j+len_square-1) << std::endl;
				current = check_point(x, y, max_iter, power);
				if (!current)
					all = false; 
			}
			for(int l = j+1; l < j + len_square -1; l++){
				y = view_y1 - i * y_stepsize;
				x = view_x0 + l * x_stepsize;
				current = check_point(x, y, max_iter, power);
				if (!current)
					all = false; 
				y = view_y1 - (i+len_square-1) * y_stepsize;
				x = view_x0 + l * x_stepsize;
				//std::cout << j <<  l << ' ' <<  (i+len_square-1) << std::endl;
				current = check_point(x, y, max_iter, power);
				if (!current)
					all = false; 
			}
			if (all){
				pointsInSetCount += (len_square-2)*(len_square-2);
			}
			else{
				for(int l = i + 1; l < i + len_square - 1;	l++){
					for(int m = j + 1; m < j + len_square - 1; m++){
						//std::cout << l << ' ' << m << std::endl;
						y = view_y1 - l * y_stepsize;
						x = view_x0 + m * x_stepsize;
						check_point(x, y, max_iter, power);
					}
				}
			}
		}
    }

    return pointsInSetCount;
}


int main(int argc, char **argv) {
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
    int numSamplesInSet = mandelbrot_draw(x_resolution, y_resolution, max_iter,
                                          view_x0, view_x1, view_y0, view_y1,
                                          x_stepsize, y_stepsize, palette_shift, image, power, no_output);
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
