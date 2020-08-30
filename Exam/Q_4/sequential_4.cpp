#include <cstdio>
#include <unistd.h>
#include <cstdlib>

#define NUM_ENTRIES 10
#define NUM_THREADS 3

struct NodeEntry {
    int value;
    NodeEntry *next;
};
NodeEntry listHead;

void outputArray(NodeEntry *entry);

NodeEntry *getLastElement(NodeEntry *element) {
    while (element->next != nullptr) {
        element = element->next;
    }
    return element;
}

// Everybody loves C89
int main(int argc, char **argv) {
    int i;
    NodeEntry *newNode, *end;

    for (i = 0; i < NUM_ENTRIES; i++) {
        // Do some intense calculation
        sleep(1);
        newNode = (NodeEntry*)malloc(sizeof(NodeEntry));
        newNode->value = i * 0b0000010000000000;

        // Insert it into our list. The order of elements is not relevant
        end = getLastElement(&listHead);
        end->next = newNode;
    }

    outputArray(&listHead);

    // Suppress unused variable compiler warning.
    (void)argc, (void)argv;

    // Nobody ever frees their memory correctly, so don't bother with it.
    return 0;
}

// Utility methods, not relevant

void outputArray(NodeEntry *entry) {
    printf("Output (order irrelevant):\n");
    while (entry != nullptr) {
        printf("%i\n", entry->value);
        entry = entry->next;
    }
    printf("\n");
}
