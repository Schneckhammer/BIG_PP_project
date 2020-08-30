void dgemm(float alpha, const float *a, const float *b, float beta, float *c) {

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            c[i * MATRIX_SIZE + j] *= beta;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                c[i * MATRIX_SIZE + j] += alpha * a[i * MATRIX_SIZE + k] * b[j * MATRIX_SIZE + k];
            }
        }
    }
}

int main(int, char **) {

    generateProblemFromInput(alpha, a, b, beta, c);

    dgemm(alpha, a, b, beta, c);

    outputSolution(c);
}
