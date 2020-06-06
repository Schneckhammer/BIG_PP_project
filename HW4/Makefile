# clang version
CXX = clang++
CXX_FLAGS = --std=c++17 -Wall -Wextra -fopenmp -pthread -mavx -fstrict-overflow -fno-tree-vectorize -L/usr/local/opt/llvm/lib -I/usr/local/opt/llvm/include

# # gcc version
# CXX = g++
# CXX_FLAGS = --std=c++17 -Wall -Wextra -fopenmp -pthread -mavx -fstrict-overflow -fno-tree-vectorize

all: student_submission sequential_implementation

student_submission: dgemm.h student_submission.cpp
	$(CXX) $(CXX_FLAGS) -o student_submission student_submission.cpp

sequential_implementation: dgemm.h sequential_implementation.cpp
	$(CXX) $(CXX_FLAGS) -o sequential_implementation sequential_implementation.cpp

clean:
	rm -f student_submission sequential_implementation