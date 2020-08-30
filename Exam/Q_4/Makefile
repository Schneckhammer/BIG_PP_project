# PLAIN_FLAGS = clang++ --std=c++17 -Wall -Wextra -O3 -L/usr/local/opt/llvm/lib -I/usr/local/opt/llvm/include
# OMP_FLAGS 	= clang++ --std=c++17 -Wall -Wextra -O3 -fopenmp -L/usr/local/opt/llvm/lib -I/usr/local/opt/llvm/include
# MPI_FLAGS	= mpic++  --std=c++17 -Wall -Wextra -O3 -openmp -pthread -mavx -g

FLAGS 		= clang++ --std=c++17 -Wall -Wextra -O3 -fopenmp -pthread -mavx -L/usr/local/opt/llvm/lib -I/usr/local/opt/llvm/include

all: parallel_4 sequential_4

sequential_4: sequential_4.cpp
	$(FLAGS) -o sequential_4 sequential_4.cpp

parallel_4: parallel_4.cpp
	$(FLAGS) -o parallel_4 parallel_4.cpp

clean:
	rm -f parallel_4 sequential_4
