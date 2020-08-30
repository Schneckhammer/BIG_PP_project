#include <mpi.h>

void count_occurrences(const unsigned char *document, std::size_t documentSize,
                        int rank, int* occurrences) {
    // static parallelization of the loop
    int portion;  // portion for each thread

    if (documentSize % NUM_PROC == 0){
        portion = (documentSize / NUM_PROC);
    }
    elsez{
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
            if (longestCommonSubsequence(query, document + startIndex, searchStringSize) >= 0.7 * searchStringSize) {
                occurrences[queryId]++;
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


    int occurrences[NUM_QUERIES] = {0};
    int rootOccurrences[NUM_QUERIES] = {0};

    // Partition is inside of count_occurences
    count_occurrences(document, DOCUMENT_SIZE, rank, occurrences);

    // Each process contains the occurences for its text chunk
    MPI_Reduce(occurrences, rootOccurrences, NUM_QUERIES, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


    if (rank==0){
        for (int queryId = 0; queryId < NUM_QUERIES; queryId++) {
            printf("Query %i: %i occurrences (query length %zu).\n",
                    queryId, rootOccurrences[queryId], Utility::getQueryLength(queryId));
        }
        std::cout << "DONE" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
