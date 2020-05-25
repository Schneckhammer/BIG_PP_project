CXX = g++
CXX_FLAGS = --std=c++17 -Wall -Wextra -fopenmp -O3

all: sequential_implementation student_submission

Utility.o: Utility.cpp
	$(CXX) $(CXX_FLAGS) -c -o Utility.o Utility.cpp

VideoOutput.o: VideoOutput.cpp
	$(CXX) $(CXX_FLAGS) -c -o VideoOutput.o VideoOutput.cpp


sequential_implementation: sequential_implementation.cpp Utility.o VideoOutput.o
	$(CXX) $(CXX_FLAGS) -o sequential_implementation sequential_implementation.cpp Utility.o VideoOutput.o

student_submission: student_submission.cpp Utility.o VideoOutput.o
	$(CXX) $(CXX_FLAGS) -o student_submission student_submission.cpp Utility.o VideoOutput.o

clean:
	rm -f sequential_implementation student_submission *.o out.mp4
