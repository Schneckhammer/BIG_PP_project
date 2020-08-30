1011 no 03, no omp
seq
3.20
seq + `c[i * MATRIX_SIZE + j] += alpha * sum;`
2.76
intrinsics # no vectorization of the remainder
0.97

1511
original seq
10.82
intrinsics
3.52
# 3.07 speedup on my pc
# 4.2 speedup on server

03 + intrinsics
0.69
O3 + omp simd
0.67

# so with O3, intrinsics are as fast as openmp for this problem

1511, 8 cores
no O3
10.82
no O3 + omp simd
11.6 # omp can’t work without compiler optimizations
O3
4.42
O3 + omp simd
0.67
O3 + omp parallel + omp simd
1.11
O3 + omp parallel
1.12
# for some reason, you can't use parallelization and simd in the same case or it just works bad

5511 + O3
omp simd
38.4
omp parallel static + omp simd
44.5
omp parallel static
44.5
# same picture here

