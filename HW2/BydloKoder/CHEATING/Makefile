all: student_submission reference_implementation generate_cheating

reference_implementation: sequential_implementation.cpp
	g++ -std=c++17 -Wall -Wextra -o sequential_implementation -O3 -g sequential_implementation.cpp -lpthread -lm

student_submission: student_submission.cpp
	g++ -std=c++17 -Wall -Wextra -o student_submission -O3 -g student_submission.cpp -lpthread -lm

generate_cheating: generate_cheating.cpp
	g++ -std=c++17 -Wall -Wextra -o generate_cheating -O3 -g generate_cheating.cpp -lpthread -lm
clean:
	rm -f sequential_implementation student_submission mandelbrot.ppm
