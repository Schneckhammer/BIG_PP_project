#include <cstring>
#include <iostream>

#include "vv-aes.h"

/**
 * This function takes the characters stored in the 4x4 message array and substitutes each character for the
 * corresponding replacement as specified in the originalCharacter and substitutedCharacter array.
 * This corresponds to step 2.1 in the VV-AES explanation.
 */
void substitute_bytes() {
    // For each byte in the message
    for (int row = 0; row < MESSAGE_BLOCK_WIDTH; row++) {
        for (int column = 0; column < MESSAGE_BLOCK_WIDTH; column++) {
			// take advantage of the fact that substitutedCharacter is sorted 
            message[row][column] = substitutedCharacter[message[row][column]];
        }
    }
}

/*
 * This function shifts (rotates) a row in the message array by row place to the left.
 * @param row The row which to shift.
 */

void shift_row(int row) {
    // This does a shift (really a rotate) of a row, copying each element to the left
	uint8_t temp[4] = {message[row][0], message[row][1], message[row][2], message[row][3]};

    for (int i = 0; i < MESSAGE_BLOCK_WIDTH; i++) {
		message[row][i % MESSAGE_BLOCK_WIDTH] = temp[(i+row) % MESSAGE_BLOCK_WIDTH]; 
    }

}

/*
 * This function shifts each row by the number of places it is meant to be shifted according to the AES specification.
 * Row zero is shifted by zero places. Row one by one, etc.
 * This corresponds to step 2.2 in the VV-AES explanation.
 */
void shift_rows() {
    // Shift each row, where the row index corresponds to how many columns the data is shifted.
    for (int row = 1; row < MESSAGE_BLOCK_WIDTH; row++) {
        shift_row(row);
    }
}

/*
 * This function calculates x^n for polynomial evaluation.
 */
inline int power(int x, int n) {
	if (n == 1)
		return x;	
	if (n == 2)
		return x*x;
	if (n == 3)
		return x*x*x;
	return x*x*x*x;
    
}

/*
 * This function evaluates four different polynomials, one for each row in the column.
 * Each polynomial evaluated is of the form
 * m'[row, column] = c[r][3] m[3][column]^4 + c[r][2] m[2][column]^3 + c[r][1] m[1][column]^2 + c[r][0]m[0][column]^1
 * where m' is the new message value, c[r] is an array of polynomial coefficients for the current result row (each
 * result row gets a different polynomial), and m is the current message value.
 *
 */

void mix_columns() {
	for (int row = 0; row < MESSAGE_BLOCK_WIDTH; row++) {
		for (int column = 0; column < MESSAGE_BLOCK_WIDTH; column++) {
			int result = 0;
			for (int degree = 0; degree < MESSAGE_BLOCK_WIDTH; degree++) {
				result += power(message[degree][column], degree + 1) * polynomialCoefficients[row][degree];
			}
			message[row][column] = result;
		}
	}
}

/*
 * For each column, mix the values by evaluating them as parameters of multiple polynomials.
 * This corresponds to step 2.3 in the VV-AES explanation.
 */

/*
 * Add the current key to the message using the XOR operation.
 */
void add_key() {
    for (int row = 0; row < MESSAGE_BLOCK_WIDTH; row++) {
        for (int column = 0; column < MESSAGE_BLOCK_WIDTH; column++) {
            // ^ == XOR
            message[row][column] = message[row][column] ^ key[row][column];
        }
    }
}


void sort(uint8_t *original, uint8_t *destination, int size){
	int min_position{};
	int min;
	for (int i = 0; i < size; i++){
		min = 500;
		for (int j = i; j < size; j++){
			if (original[j] < min){
				min = original[j];
				min_position = j;
			}
		}
		if (i != min_position){		
			std::swap(original[i], original[min_position]);
			std::swap(destination[i], destination[min_position]);
		}
	}
}


/*
 * Your main encryption routine.
 */
int main() {
	sort(originalCharacter, substitutedCharacter, UNIQUE_CHARACTERS);

    // Receive the problem from the system.
    readInput();


    // For extra security (and because Varys wasn't able to find enough test messages to keep you occupied) each message
    // is put through VV-AES lots of times. If we can't stop the adverse Maesters from decrypting our highly secure
    // encryption scheme, we can at least slow them down.
    for (int i = 0; i < 500000; i++) {
        // For each message, we use a predetermined key (e.g. the password). In our case, its just pseudo random.
        set_next_key();

        // First, we add the key to the message once:
        add_key();

        // We do 9+1 rounds for 128 bit keys
        for (int round = 0; round < 9; round++) {
            //In each round, we use a different key derived from the original (refer to the key schedule).
            set_next_key();

            // These are the four steps described in the slides.
            substitute_bytes();
            shift_rows();
            mix_columns();
            add_key();
        }
        // Final round
        substitute_bytes();
        shift_rows();
        add_key();
    }
    // Submit our solution back to the system.
    writeOutput();
    return 0;
}
