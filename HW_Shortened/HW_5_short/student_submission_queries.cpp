//
// Created by Vincent Bode on 23/06/2020.
//

#include <cstddef>
#include <cstdio>
#include <algorithm>
#include <iostream>
#include <mpi.h>
#include "Utility.h"
#include "StringSearch.h"

int count_occurrences(const unsigned char *searchString, std::size_t searchStringSize,
                      const unsigned char *document, std::size_t documentSize) {
    int occurrences = 0;
    // Search from every possible start string position and determine whether there is a match.
    for (std::size_t startIndex = 0; startIndex < documentSize - searchStringSize; ++startIndex) {
        // We consider 2 sequences to match if the longest common subsequence contains >= 70% the number of characters
        // of the query. In that case, they are close enough so that we count them as the same.
        if (longestCommonSubsequence(searchString, document + startIndex, searchStringSize) >= 0.7 * searchStringSize) {
            occurrences++;
        }
    }
    return occurrences;
}

int main(int argc, char **argv) {
    unsigned char* document;
    Utility::readEncyclopedia(document);

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0) {
        Utility::generateProblemFromInput(document);
    }
    MPI_Bcast(Utility::getQueryBuffer(), NUM_QUERIES * MAX_QUERY_LENGTH, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

    int chunkSize = (NUM_QUERIES / size) + 1;
    int occurrences[NUM_QUERIES] = {0};
    int rootOccurrences[NUM_QUERIES] = {0};
    
    // Partition is inside of the loop (2 queries per process of smth)

    for (int queryId = chunkSize * rank; queryId < std::min(chunkSize * (rank + 1), NUM_QUERIES); queryId++) {
        unsigned char *query = Utility::getQuery(queryId);
        std::size_t queryLength = Utility::getQueryLength(queryId);
        occurrences[queryId] = count_occurrences(query, queryLength, document, DOCUMENT_SIZE);
    }

    // MAX because all except one contain zeros, and one contains the answer
    // root occurences is the array where we save the results
    MPI_Reduce(occurrences, rootOccurrences, NUM_QUERIES, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        for (int queryId = 0; queryId < NUM_QUERIES; queryId++) {
            printf("Query %i: %i occurrences (query length %zu).\n", queryId, rootOccurrences[queryId], Utility::getQueryLength(queryId));
        }
        std::cout << "DONE" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
