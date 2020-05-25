#ifndef ASSIGNMENTS_UTILITY_H
#define ASSIGNMENTS_UTILITY_H

#include "a-star-navigator.h"
#include <vector>

class Utility {
public:

    static void writeMap(unsigned int seed, ProblemData& problemData);

    static void writeInitialStormData(unsigned int seed, ProblemData& problemData);

    static void generateProblem(unsigned int seed, ProblemData &problemData);

    static unsigned int readInput();

    // Output the solution to a problem
    static void writeOutput(int pathLength);

    // This will stop the timer. No more output will be accepted after this call.
    static void stopTimer();
};


#endif //ASSIGNMENTS_UTILITY_H
