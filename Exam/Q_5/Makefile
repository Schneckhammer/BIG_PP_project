MPI_FLAGS	= mpic++  --std=c++17 -Wall -Wextra -O3 -fopenmp -pthread -mavx -g  -L/usr/local/opt/llvm/lib -I/usr/local/opt/llvm/include
FLAGS	= clang++  --std=c++17 -Wall -Wextra -O3 -fopenmp -pthread -mavx -g  -L/usr/local/opt/llvm/lib -I/usr/local/opt/llvm/include

all: parallel_5 sequential_5

sequential_5: sequential_5.cpp
	$(FLAGS) -o sequential_5 sequential_5.cpp

parallel_5: parallel_5.cpp
	$(MPI_FLAGS) -o parallel_5 parallel_5.cpp

clean:
	rm -f parallel_5 sequential_5
