

int mandelbrot_draw(int x_resolution, int y_resolution, int max_iter,
                    double view_x0, double view_x1, double view_y0, double view_y1,
                    double x_stepsize, double y_stepsize,
                    int palette_shift, unsigned char *img,
                    double power, bool no_output) {

    int pointsInSetCount = 0;

    // For each pixel in the image
    for (int i = 0; i < y_resolution; i++) {
        for (int j = 0; j < x_resolution; j++) {
            do_stuff();

            if (k == max_iter) {
                pointsInSetCount ++;
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
    double power = 2.0;
    int max_iter = 1000;
    int x_resolution = 1033;
    int y_resolution = 1033;

    int palette_shift = 0;
    double view_x0 = -2;
    double view_x1 = +2;
    double view_y0 = -2;
    double view_y1 = +2;
    char file_name[256] = "mandelbrot.ppm";
    int no_output = 0;

    double x_stepsize;
    double y_stepsize;

    // Calculate the step size.
    x_stepsize = (view_x1 - view_x0) / (1.0 * x_resolution);
    y_stepsize = (view_y1 - view_y0) / (1.0 * y_resolution);

    getProblemFromInput(&power);


    int numSamplesInSet = mandelbrot_draw(x_resolution, y_resolution, max_iter,
                                          view_x0, view_x1, view_y0, view_y1,
                                          x_stepsize, y_stepsize, palette_shift, image, power, no_output);
    outputSolution(numSamplesInSet);

    
    free(image);
    return 0;
}
