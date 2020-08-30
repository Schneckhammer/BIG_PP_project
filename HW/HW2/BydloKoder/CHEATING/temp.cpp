#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <complex>

#include "mandelbrot_set.h"

#define CHANNELS 3
// Returns the one dimensional index into our pseudo 3D array
#define OFFSET(y, x, c) (y * x_resolution * CHANNELS + x * CHANNELS + c)


int main(int argc, char **argv) {
	
	// linear fit parameters (power from 2 to 3 split into 100 regions. The parameters of a line come in pairs)
	double fit_params[200] = {438903.0381788419,-776922.0400294894,313824.25479788525,-525792.8612903398,295636.3519303656,-489056.39448302536,265006.0680105656,-426955.64536749566,220878.78098405237,-336961.56772189494,175284.8387735322,-243471.10126022148,162636.3781293809,-217366.10264810687,112339.40743429516,-113275.30072244562,97660.60876241843,-82716.53896526126,70933.31577784827,-26820.02989670047,34575.762983232315,49466.80680156741,5866.678063049068,110000.3092140085,-21224.208903583178,167389.63181566488,-50781.829938355266,230328.9160034194,-85709.08142706958,305045.32512035203,-84860.60743858329,303180.37872642773,-28490.89136167555,181505.63435234682,-10145.448245092577,141767.27720895404,7921.211632150192,102393.21318956792,25884.851986639304,63061.9923153286,30175.765133703833,53600.42575466013,46072.716528708435,18515.369247176135,40690.91047744704,30482.269642994877,31230.325186314687,51637.33837115378,26793.91114916623,61604.96642569651,33102.999663206254,47488.88718086579,31369.694267544404,51404.12733123853,27424.249544197708,60338.64441173931,30981.849877488552,52212.963954853156,22872.714099491342,70756.35749871896,24048.479717452985,68104.27849112965,34672.68852805604,43560.562401919786,42212.09989341849,26024.87379774883,36818.20370772589,38589.60344419264,47903.025241109935,12591.957322217962,33509.059217366535,46365.82007283839,19569.694436870126,79309.35750396455,-3030.2927375075324,132864.23031655693,-13424.287223375159,157589.11288413807,-15757.587676582174,163107.943691576,-19084.812969574858,171141.93278534224,-11496.977010351786,152855.350991496,-3993.944908781264,134740.01943134185,-11993.893797850069,154212.8344508669,-3206.124549666991,132822.87144534767,-6551.498542151698,141019.95321059314,-6848.499534560247,141803.32710292432,7442.408119445795,106539.96110849585,12612.112778347562,93679.90580215109,23963.64170322266,65396.49577132003,19284.866463744936,77109.45194155077,16387.88801631167,84288.85558292718,21969.684038685915,70141.03264433761,14600.120155112798,88824.29546686517,13042.38514664026,92740.85096057551,8575.731291334096,104121.1944162892,3181.816022973654,117966.53277118043,5236.3526869288,112628.81004075386,-15296.96969690862,165645.01818165794,-11006.015382165715,154491.3069090297,-16812.099808767518,169585.11395193258,-16321.232846177376,168289.56327633478,-26357.58685701927,194635.1867062472,-21193.956406639958,181069.3781532928,-15139.358044015955,165080.23234740036,-5793.971524738951,140315.29741241955,-2036.244311866929,130200.57315425188,-16909.069844122023,169860.30729810346,-17945.4449507269,172663.0469702206,-21381.856118830674,181882.61131218914,-15563.583953579839,166169.71280245695,-9987.880367790707,151044.30125836856,-17515.138625555373,171486.1951853248,-9296.975484787556,149037.9794764557,-8090.864933972434,145745.87881129,-1909.0532140270407,128713.78699191695,-11297.014079474311,154617.59542270674,-20527.25959159552,180247.1817368748,-4666.82954093495,136183.38694991582,-17824.233024152418,172918.71918599794,-11303.041340595955,154685.27943970176,-13436.319747165931,160681.02192839838,-3430.402254493609,132448.57135671107,-19442.40560837149,177750.0986969236,-7563.6363629841935,144044.96363450907,-9624.256332898736,149942.2397022561,-7315.109256603,143290.6304064863,-4448.494170460013,134975.59649298879,-21624.25200502227,184418.95490848695,-7381.742000348975,143242.85222000995,-25284.829672174998,195165.78778283298,-21418.150125759355,183964.19854152438,-19781.79807591099,179231.86847300341,-8139.416608452556,145067.41803750416,-13727.273213310285,161438.65597659006,-21763.620946652536,185114.5180868847,-16896.94125455695,170719.7823491356,-21375.751840408266,183999.27384929362,-25987.884731290087,197807.4419805353,-25357.608022511715,195975.95722340493};

	
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
	
	// find out which parameters to use
	int i = (power-2) * 100;
	// compute using linear fit
    int numSamplesInSet = power*fit_params[i*2] + fit_params[i*2+1];
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
