

void simulate_waves(ProblemData &problemData, int t) {
    auto &islandMap = problemData.islandMap;
    float (&secondLastWaveIntensity)[MAP_SIZE][MAP_SIZE] = *problemData.secondLastWaveIntensity;
    float (&lastWaveIntensity)[MAP_SIZE][MAP_SIZE] = *problemData.lastWaveIntensity;
    float (&currentWaveIntensity)[MAP_SIZE][MAP_SIZE] = *problemData.currentWaveIntensity;

    for (int x = 1; x < MAP_SIZE - 1; ++x) {
        for (int y = 1; y < MAP_SIZE - 1; ++y) {
            do_stuff();
        }
    }
}

bool findPathWithExhaustiveSearch(ProblemData &problemData, int timestep,
                                  std::vector<Position2D> &pathOutput) {

    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {

            for (Position2D &neighbor: neighbors) {
                numPossiblePositions++;
            }
        }
    }
    return false;
}

int main(int argc, char *argv[]) {

    for (int problem = 0; problem < numProblems; ++problem) {

        Utility::generateProblem(seed + problem, *problemData);

        for (int t = 2; t < TIME_STEPS; t++) {
            simulate_waves(*problemData, t);
        }

        Utility::writeOutput();
    }
}