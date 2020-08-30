#include <omp.h>

void simulate_waves(ProblemData &problemData, int t) {
    auto &islandMap = problemData.islandMap;
    float (&secondLastWaveIntensity)[MAP_SIZE][MAP_SIZE] = *problemData.secondLastWaveIntensity;
    float (&lastWaveIntensity)[MAP_SIZE][MAP_SIZE] = *problemData.lastWaveIntensity;
    float (&currentWaveIntensity)[MAP_SIZE][MAP_SIZE] = *problemData.currentWaveIntensity;

    // Number of nested threads
	// #pragma omp parallel for schedule(guided) num_threads(4)
    #pragma omp parallel for schedule(dynamic, 32) num_threads(4)
    for (int x = 1; x < MAP_SIZE - 1; ++x) {
        for (int y = 1; y < MAP_SIZE - 1; ++y) {
            do_stuff();
        }
    }
}


bool findPathWithExhaustiveSearch(ProblemData &problemData, int timestep,
                                  std::vector<Position2D> &pathOutput) {
	bool found = false;

	#pragma omp for 
    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {

            for (Position2D &neighbor: neighbors) {
                if (neighborPosition == portRoyal) {
					found = true;
                }
                numPossiblePositions++;
            }
        }
    }
    return found;
}

int main(int argc, char *argv[]) {

    // omp_set_nested(2); done in simulate_waves directly
    #pragma omp parallel for ordered num_threads(6)
    for (int problem = 0; problem < numProblems; ++problem) {

		#pragma omp critical
		{
        Utility::generateProblem(seed + problem, *problemData);
		}

        for (int t = 2; t < TIME_STEPS; t++) {
            simulate_waves(*problemData, t);
        }
        
        #pragma omp ordered
        {
            Utility::writeOutput();
        }
    }
}
