//
// Created by Vincent Bode on 23/06/2020.
//

#include <mpi.h>
//#include <stdio.h>
#include <cstddef>
#include <cstdio>
#include <algorithm>
#include "Utility.h"
#include "StringSearch.h"

int longestCommonSubsequence(const unsigned char* str1, const unsigned char* str2, size_t len);

// Routine where we match our input sequence across the entire document and return the number of matches.
// If you're just splitting the work by dividing the queries, then you don't need to touch this function.
int count_occurrences(const unsigned char *searchString, std::size_t searchStringSize,
                      const unsigned char *document, std::size_t documentSize,
                      int rank) {
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

int main(int, char **) {
    unsigned char* document;
    Utility::readEncyclopedia(document);

    int size, rank;
    int result[NUM_QUERIES];

    MPI_Init (NULL, NULL);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);

    if (rank==0) {
        Utility::generateProblemFromInput(document);
    }

    MPI_Bcast(Utility::getQueryBuffer(), NUM_QUERIES*MAX_QUERY_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);

    unsigned char *query = Utility::getQuery(rank);
    std::size_t queryLength = Utility::getQueryLength(rank);
    int occurrences = count_occurrences(query, queryLength, document, DOCUMENT_SIZE);

    MPI_Gather(&occurrences, 1, MPI_INT, result, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank==0){
        for (int queryId = 0; queryId < NUM_QUERIES; queryId++) {
            printf("Query %i: %i occurrences (query length %zu).\n",
                    queryId, result[queryId], Utility::getQueryLength(queryId));
        }
    }

    MPI_Finalize ();

    return 0;
}

int cache[MAX_QUERY_LENGTH + 1][MAX_QUERY_LENGTH + 1];
// We use the longest common subsequence as our metric for string similarity. Both buffers must have at least length len.
// Note that this is different from the longest common substring. If you just want to parallelize, you do not need to
// touch this implementation.
// https://en.wikipedia.org/wiki/Longest_common_subsequence_problem
int longestCommonSubsequence(const unsigned char* str1, const unsigned char* str2, size_t len) {
    for (unsigned int i = 0; i <= len; ++i) {
        cache[i][0] = 0;
        cache[0][i] = 0;
    }
    for(unsigned int i = 1; i <= len; i++) {
        for(unsigned int j = 1; j <= len; j++)
        if(str1[i] == str2[j]) {
            cache[i][j] = cache[i - 1][j - 1] + 1;
        } else {
            cache[i][j] = std::max(cache[i][j - 1], cache[i - 1][j]);
        }
    }
    return cache[len][len];
}
