int count_occurrences(const unsigned char *searchString, std::size_t searchStringSize,
                      const unsigned char *document, std::size_t documentSize) {
    int occurrences = 0;
    for (std::size_t startIndex = 0; startIndex < documentSize - searchStringSize; ++startIndex) {
        if (longestCommonSubsequence(searchString, document + startIndex, searchStringSize) >= 0.7 * searchStringSize) {
            occurrences++;
        }
    }
    return occurrences;
}

int main(int, char **) {
    unsigned char* document;
    Utility::readEncyclopedia(document);

    Utility::generateProblemFromInput(document);

    for (int queryId = 0; queryId < NUM_QUERIES; queryId++) {

        unsigned char *query = Utility::getQuery(queryId);
        std::size_t queryLength = Utility::getQueryLength(queryId);

        int occurrences = count_occurrences(query, queryLength, document, DOCUMENT_SIZE);

        printf("Query %i: %i occurrences (query length %zu).\n", 
                queryId, occurrences, queryLength);
    }

    return 0;
}
