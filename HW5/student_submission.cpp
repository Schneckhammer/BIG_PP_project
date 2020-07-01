#include <mpi.h>
#include <cstddef>
#include <cstdio>
#include <algorithm>
#include <iostream>
#include <chrono>
#include "Utility.h"
#include "StringSearch.h"

#define NUM_PROC 16

int longestCommonSubsequence(const unsigned char* str1, const unsigned char* str2, size_t len);
int commonSubset(const unsigned char* str1, const unsigned char* str2, size_t len);


void count_occurrences(const unsigned char *document, std::size_t documentSize,
                        int rank, int* occurrences_es) {

    // static parallelization of the loop
    int portion;  // portion for each thread

    if (documentSize % NUM_PROC == 0){
        portion = (documentSize / NUM_PROC);
    }
    else{
        portion = (documentSize / NUM_PROC) + 1;
    }
    std::size_t startIndex = portion * rank;
    std::size_t endIndex = portion * (rank+1);
    // if this is the last process, don't go over the end
    if (rank == NUM_PROC-1){
        endIndex = documentSize;
    }

    for (int queryId = 0; queryId < NUM_QUERIES; queryId++) {
        std::size_t searchStringSize = Utility::getQueryLength(queryId);
        unsigned char *query = Utility::getQuery(queryId);
        for (std::size_t start = startIndex; start < endIndex - searchStringSize; ++start) {     
            // first check how much common chars the 2 strings have. 
            // Finding the size of the subset is a lot faster than the size of subsequence.
             int comm_subset = commonSubset(
                     query,
                     document + start,
                     searchStringSize);
             if ( comm_subset >= 0.7 * searchStringSize){
                 // if strings have enough common chars, try the longest subsequence
                if (longestCommonSubsequence(
                    query,
                    document + start,
                    searchStringSize) >= 0.7 * searchStringSize) {
                    occurrences_es[queryId]++;
                }
             }
             else {
                 // if strings have little in common, we can skip several iterations
                 int jump = std::max(0, (int(0.7 * searchStringSize) - comm_subset));
                 start = start + jump;
             }
        }        
    }
}

int main(int, char **) {

    unsigned char* document;
    Utility::readEncyclopedia(document);
    
    int size, rank;

    MPI_Init (NULL, NULL);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);


    if (rank==0) {
        Utility::generateProblemFromInput(document);
    }
    MPI_Bcast(Utility::getQueryBuffer(), NUM_QUERIES*MAX_QUERY_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);


    int occurrences_es[16] = {0};
    count_occurrences(document, DOCUMENT_SIZE, rank, occurrences_es);

    int result[16];
    MPI_Reduce(occurrences_es, result, 16, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank==0){
        for (int queryId = 0; queryId < NUM_QUERIES; queryId++) {
            printf("Query %i: %i occurrences (query length %zu).\n",
                    queryId, result[queryId], Utility::getQueryLength(queryId));
        }
    }

    MPI_Finalize();
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
        for(unsigned int j = 1; j <= len; j++){
            if(str1[i] == str2[j]) {
                cache[i][j] = cache[i - 1][j - 1] + 1;
            } else {
                cache[i][j] = std::max(cache[i][j - 1], cache[i - 1][j]);
            }
        }
    }
    return cache[len][len];
}


// this function is at least 20 times faster than the longestCommonSubsequence
int commonSubset(const unsigned char* str1, const unsigned char* str2, size_t len) {
    int common_chars = 0;
    for (unsigned int i = 1; i <= len; i++) {
        for(unsigned int j = 1; j <= len; j++){
            if(str1[i] == str2[j]) {
                common_chars++;
                break;
            }
        }
    }
    return common_chars;
}
