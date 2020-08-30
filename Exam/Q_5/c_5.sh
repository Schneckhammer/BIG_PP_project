#!/usr/local/bin/bash
make

command_a="./sequential_5"
command_b="mpirun -n 16 --oversubscribe ./parallel_5"

printf "\n-----------------------\n    RUNNING SEQUENTIAL\n-----------------------\n"
$command_a | tee a_output.txt
printf "\n-----------------------\n    RUNNING PARALLEL  \n-----------------------\n"
$command_b | tee b_output.txt
printf "\n-----------------------\n    DIFFERENCE        \n-----------------------\n"
diff a_output.txt b_output.txt

# chmod +x c_5.sh && ./c_5.sh

Result 997: 1113679758
Result 998: 874030783
Result 999: 1468227371


Result 957: 647440705
Result 984: 732446223
Result 985: 1024734775