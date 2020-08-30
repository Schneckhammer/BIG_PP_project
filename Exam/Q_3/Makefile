# PLAIN_FLAGS = clang++ --std=c++17 -Wall -Wextra -O3 -L/usr/local/opt/llvm/lib -I/usr/local/opt/llvm/include
# OMP_FLAGS 	= clang++ --std=c++17 -Wall -Wextra -O3 -fopenmp -L/usr/local/opt/llvm/lib -I/usr/local/opt/llvm/include
# MPI_FLAGS	= mpic++  --std=c++17 -Wall -Wextra -O3 -openmp -pthread -mavx -g

FLAGS 		= clang++ --std=c++17 -Wall -Wextra -O3 -fopenmp -pthread -mavx -L/usr/local/opt/llvm/lib -I/usr/local/opt/llvm/include

all: parallel_3 sequential_3

sequential_3: sequential_3.cpp
	$(FLAGS) -o sequential_3 sequential_3.cpp

parallel_3: parallel_3.cpp
	$(FLAGS) -o parallel_3 parallel_3.cpp

clean:
	rm -f parallel_3 sequential_3
