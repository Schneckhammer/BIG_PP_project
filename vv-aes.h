//
// Created by Vincent Bode on 21/04/2020.
//

#ifndef ASSIGNMENTS_VV_AES_H
#define ASSIGNMENTS_VV_AES_H

#include <limits>

#define MESSAGE_BLOCK_WIDTH 4
#define UNIQUE_CHARACTERS 256
#define NUM_ROUNDS 10

/*
 * This is the message given to you to encrypt to verify the correctness and speed of your approach. Unfortunately,
 * there are no state secrets to be found here. Those would have probably made a pretty penny on the black market.
 */
uint8_t message[MESSAGE_BLOCK_WIDTH][MESSAGE_BLOCK_WIDTH] = {
        {'M', 'A', 'S', 'T' },
        {'E', 'R', ' ', 'W'},
        {'H', 'I', 'S', 'P'},
        {'E', 'R', 'E', 'R'}
};

/*
 * The set of all keys generated at runtime and the index of the current key.
 */
int currentKey = 0;
uint8_t allKeys[NUM_ROUNDS][MESSAGE_BLOCK_WIDTH][MESSAGE_BLOCK_WIDTH];

/*
 * Use this like a 2D-Matrix key[MESSAGE_BLOCK_WIDTH][MESSAGE_BLOCK_WIDTH];
 * The key is only handed to you when it's time to actually encrypt something.
 */
uint8_t (*key)[MESSAGE_BLOCK_WIDTH];


/*
 * This is the source list of characters. If you wish to translate a character, find its index in this list. The
 * corresponding output character resides at the same index in the substituted character list.
 */
uint8_t originalCharacter[] = { 210, 223, 147, 111, 187, 107, 193, 11, 109, 246, 198, 89, 133, 135, 192, 227, 99, 66, 160, 69, 110, 48, 228, 79, 108, 142, 186, 165, 206, 129, 28, 150, 117, 97, 171, 58, 178, 244, 62, 30, 84, 24, 243, 23, 35, 76, 182, 226, 155, 188, 184, 105, 149, 177, 26, 161, 74, 127, 159, 18, 234, 1, 94, 65, 185, 39, 41, 236, 220, 156, 88, 21, 128, 213, 73, 154, 55, 37, 203, 7, 90, 103, 175, 86, 254, 174, 233, 172, 25, 2, 248, 199, 218, 68, 191, 235, 216, 102, 140, 121, 32, 115, 162, 71, 224, 138, 22, 95, 249, 20, 229, 196, 67, 197, 225, 241, 98, 17, 101, 136, 118, 141, 217, 252, 93, 130, 82, 122, 219, 96, 237, 176, 57, 50, 114, 195, 64, 126, 240, 113, 85, 146, 52, 6, 77, 201, 80, 151, 179, 144, 190, 100, 78, 119, 33, 92, 43, 38, 47, 5, 13, 56, 63, 251, 81, 167, 104, 3, 180, 205, 46, 204, 164, 143, 14, 250, 230, 59, 194, 148, 49, 19, 145, 120, 83, 4, 211, 168, 112, 44, 31, 242, 231, 202, 53, 181, 245, 9, 131, 209, 51, 123, 12, 207, 212, 208, 200, 54, 72, 163, 221, 253, 158, 0, 106, 157, 125, 29, 214, 116, 247, 16, 42, 15, 134, 222, 215, 169, 173, 170, 183, 27, 124, 139, 61, 153, 238, 70, 255, 8, 189, 91, 40, 36, 132, 239, 34, 152, 75, 87, 232, 60, 45, 10, 166, 137 };
/*
 * This is the output list of characters. If you wish to translate a character, find its index in the original list.
 * The corresponding output character resides at the same index in this list.
 */
uint8_t substitutedCharacter[] = { 109, 244, 230, 56, 18, 232, 199, 81, 116, 213, 11, 208, 188, 79, 209, 91, 19, 71, 99, 131, 36, 9, 185, 186, 80, 133, 159, 245, 15, 173, 118, 26, 190, 59, 222, 197, 95, 76, 182, 216, 87, 106, 10, 157, 24, 64, 107, 13, 211, 205, 35, 78, 142, 21, 105, 39, 69, 179, 113, 33, 161, 67, 252, 212, 137, 145, 128, 100, 125, 127, 165, 147, 242, 183, 122, 207, 121, 219, 202, 154, 61, 254, 28, 250, 236, 174, 101, 246, 255, 251, 54, 126, 206, 112, 52, 139, 86, 16, 1, 248, 152, 41, 88, 243, 130, 192, 46, 184, 0, 96, 3, 97, 34, 234, 239, 6, 90, 98, 134, 44, 215, 114, 135, 231, 191, 167, 2, 50, 148, 8, 187, 12, 94, 72, 23, 166, 193, 240, 25, 85, 196, 214, 84, 226, 117, 29, 181, 83, 151, 178, 120, 129, 31, 176, 201, 27, 65, 102, 218, 180, 73, 200, 189, 70, 247, 49, 233, 220, 43, 68, 82, 203, 144, 143, 253, 168, 227, 32, 42, 53, 170, 20, 111, 48, 103, 169, 30, 57, 5, 4, 198, 156, 92, 115, 14, 158, 160, 204, 195, 17, 217, 89, 62, 225, 7, 93, 60, 221, 66, 172, 51, 141, 140, 77, 104, 210, 123, 132, 38, 22, 74, 110, 249, 150, 175, 238, 194, 63, 149, 177, 224, 223, 55, 229, 108, 171, 237, 40, 163, 136, 162, 58, 241, 37, 138, 119, 155, 124, 153, 164, 146, 235, 45, 47, 228, 75 };

uint8_t polynomialCoefficients[MESSAGE_BLOCK_WIDTH][MESSAGE_BLOCK_WIDTH] = {
        { 3, 2, 3, 2},
        { 2, 2, 2, 1},
        { 1, 2, 3, 2},
        { 1, 2, 2, 1}
};

/*
 * Generate some keys that can be used for encryption based on the seed set from the input.
 */
void generate_keys() {
    // Fill the key block
    for(auto & currentKey : allKeys) {
        for (auto & row : currentKey) {
            for (unsigned char & column : row) {
                column = rand() % std::numeric_limits<uint8_t>::max();
            }
        }
    }
}

void readInput() {
    std::cout << "READY" << std::endl;
    unsigned int seed = 0;
    std::cin >> seed;

    std::cerr << "Using seed " << seed << std::endl;
    if(seed == 0) {
        std::cerr << "Warning: default value 0 used as seed." << std::endl;
    }

    // Set the pseudo random number generator seed used for generating encryption keys
    srand(seed);

    generate_keys();
}

void writeOutput() {
    // Output the current state of the message in hexadecimal.
    for (int row = 0; row < MESSAGE_BLOCK_WIDTH; row++) {
        std::cout << std::hex << (int) message[row][0] << (int) message[row][1] << (int) message[row][2]
                  << (int) message[row][3];
    }
    // This stops the timer.
    std::cout << std::endl << "DONE" << std::endl;
}

/*
 * This is a utility method. It determines the next key to use from the set of pre-generated keys. In a real
 * cryptographic system, the subsequent keys are generated from a key schedule from the original key. To keep the code
 * short, we have omitted this behavior.
 */
void set_next_key() {
    key = &allKeys[currentKey][0];
    currentKey = (currentKey + 1) % NUM_ROUNDS;
}

#endif //ASSIGNMENTS_VV_AES_H
