#include <omp.h>
#include <immintrin.h>

void dgemm(float alpha, const float *a, const float *b, float beta, float *c) {

    // omp_set_num_threads(8);
    // #pragma omp parallel for
    for (int i = 0; i < MATRIX_SIZE; i++) {
        // #pragma omp simd
        for (int j = 0; j < MATRIX_SIZE; j++) {
            float sum = 0;
            float vec_sum[8];
            __m256 va, vb, vprod, vsum;
            int limit = MATRIX_SIZE - (MATRIX_SIZE % 8);

            // initialize vsum with zeroes
            float zero = 0;
            vsum = _mm256_broadcast_ss(&zero);

            // do the loop
            for (int k = 0; k < limit; k+=8) {
                va = _mm256_loadu_ps(&a[i * MATRIX_SIZE + k]);
                vb = _mm256_loadu_ps(&b[j * MATRIX_SIZE + k]);
                vprod = _mm256_mul_ps(va, vb);
                vsum =  _mm256_add_ps(vsum, vprod);
            }

            // add the resulting vsum to sum
            _mm256_storeu_ps(vec_sum, vsum);
            for (int k = 0; k < 8; k++) {
                sum += vec_sum[k];
            }

            // sum over the remainder elements
            for (int k = limit; k < MATRIX_SIZE; k++) {
                sum += a[i * MATRIX_SIZE + k] * b[j * MATRIX_SIZE + k];
            }

            c[i * MATRIX_SIZE + j] *= beta;
            c[i * MATRIX_SIZE + j] += alpha * sum;
        }
    }
}

int main(int, char **) {

    generateProblemFromInput(alpha, a, b, beta, c);

    dgemm(alpha, a, b, beta, c);

    outputSolution(c);
}
