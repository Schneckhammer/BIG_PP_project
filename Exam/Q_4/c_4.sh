#!/usr/local/bin/bash
make

command_a="./sequential_4"
command_b="./parallel_4"

printf "\n-----------------------\n    RUNNING SEQUENTIAL\n-----------------------\n"
$command_a | tee a_output.txt
printf "\n-----------------------\n    RUNNING PARALLEL  \n-----------------------\n"
$command_b | tee b_output.txt
printf "\n-----------------------\n    DIFFERENCE        \n-----------------------\n"
diff a_output.txt b_output.txt

# chmod +x c_4.sh && ./c_4.sh