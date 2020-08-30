#include <algorithm>
#include "Utility.h"
#include "lib/PerlinNoise.hpp"
#include "a-star-navigator.h"

void Utility::writeMap(unsigned int seed, ProblemData &problemData) {
    const siv::PerlinNoise perlinNoise(seed);
#pragma omp parallel for schedule(static, 32)
    for (int x = 0; x < MAP_SIZE; x++) {
        for (int y = 0; y < MAP_SIZE; y++) {
            problemData.islandMap[x][y] = perlinNoise.accumulatedOctaveNoise2D_0_1(x * FREQUENCY_WIND / MAP_SIZE,
                                                                                   y * FREQUENCY_WIND / MAP_SIZE,
                                                                                   OCTAVES_WAVES);
//            islandMap[x][y] = 0.9f;
        }
    }
}

void Utility::writeInitialStormData(unsigned int seed, ProblemData &problemData) {
    unsigned int seeds[] = {seed * 2, seed / 2};
    const siv::PerlinNoise perlinNoise(seeds[0]);
#pragma omp parallel for schedule(static, 32)
    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {
            if (problemData.islandMap[x][y] >= LAND_THRESHOLD) {
                problemData.waveIntensity[0][x][y] = 0.0f;
                problemData.waveIntensity[1][x][y] = 0.0f;
            } else {
                // Make sure the gradient near coasts is not too large (don't create tsunamies).
                problemData.waveIntensity[0][x][y] =
                        perlinNoise.accumulatedOctaveNoise2D_0_1(x * FREQUENCY_WAVES / MAP_SIZE,
                                                                 y * FREQUENCY_WAVES / MAP_SIZE, OCTAVES_WAVES)
                        * std::clamp(4.0f * (LAND_THRESHOLD - problemData.islandMap[x][y]), 0.0f, 1.0f);
                problemData.waveIntensity[1][x][y] = problemData.waveIntensity[0][x][y];
            }
        }
    }
}

void Utility::generateProblem(unsigned int seed, ProblemData &problemData) {
    std::cerr << "Using seed " << seed << std::endl;
    if (seed == 0) {
        std::cerr << "Warning: default value 0 used as seed." << std::endl;
    }

    // Set the pseudo random number generator seed used for generating encryption keys
    srand(seed);

    writeMap(seed, problemData);
    writeInitialStormData(seed, problemData);

    // Ensure that the destination doesn't go out of bounds and that the ship doesn't start on land
    do {
        problemData.shipOrigin = Position2D(DISTANCE_TO_PORT + (rand() % (MAP_SIZE - 2 * DISTANCE_TO_PORT)),
                                            DISTANCE_TO_PORT + (rand() % (MAP_SIZE - 2 * DISTANCE_TO_PORT)));
    } while (problemData.islandMap[problemData.shipOrigin.x][problemData.shipOrigin.y] >= LAND_THRESHOLD);

    // Same for Port Royal
    do {
        int horizontalDistance = rand() % DISTANCE_TO_PORT;
        int verticalDistance = DISTANCE_TO_PORT - horizontalDistance;
        if (rand() % 2 == 1) { horizontalDistance = -horizontalDistance; }
        if (rand() % 2 == 1) { verticalDistance = -verticalDistance; }
        problemData.portRoyal = problemData.shipOrigin + Position2D(horizontalDistance, verticalDistance);
    } while (problemData.islandMap[problemData.portRoyal.x][problemData.portRoyal.y] >= LAND_THRESHOLD);

}

unsigned int Utility::readInput() {
    unsigned int seed = 0;
    std::cout << "READY" << std::endl;
    std::cin >> seed;

    return seed;
}

void Utility::writeOutput(int pathLength) {
    if (pathLength == -1) {
        std::cout << "Path length: no solution." << std::endl;
    } else {
        std::cout << "Path length: " << pathLength << std::endl;
    }
}

void Utility::stopTimer() {
    std::cout << "DONE" << std::endl;
}