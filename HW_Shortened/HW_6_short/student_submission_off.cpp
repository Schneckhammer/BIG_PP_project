#include <mpi.h>
#include <iostream>

#define CODE_VERSION 1

int local_counter;

struct positions {
        int start; 
        int finish;
    };

positions determine_indexes(int rank)
{
    positions indexes;

    if(rank==14){
        indexes.start = 94*rank+1;
        indexes.finish = 94*rank+94;
    }
    else if(rank == 15){
        indexes.start = 94*rank;
        indexes.finish = GRID_SIZE - 1;
    }
    else{
        indexes.start = 94*rank+1;
        indexes.finish = 94*rank+95;
    }

    return indexes;
}
void pass_messages(ProblemData& problemData, int rank)
{
    if (rank == 0) {
        MPI_Send(&(*problemData.writeGrid)[94][0], GRID_SIZE, MPI_C_BOOL, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&(*problemData.writeGrid)[95][0], GRID_SIZE, MPI_C_BOOL, 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&(*problemData.writeGrid)[1][0], GRID_SIZE, MPI_C_BOOL, 15, 0, MPI_COMM_WORLD);
        MPI_Recv(&(*problemData.writeGrid)[0][0], GRID_SIZE, MPI_C_BOOL, 15, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } 


    else if (rank % 2 == 0 && rank != 0 && rank != 14) {
        MPI_Send(&(*problemData.writeGrid)[94*rank+94][0], GRID_SIZE, MPI_C_BOOL, rank + 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&(*problemData.writeGrid)[94*rank+95][0], GRID_SIZE, MPI_C_BOOL, rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&(*problemData.writeGrid)[94*rank+1][0], GRID_SIZE, MPI_C_BOOL, rank-1, 0, MPI_COMM_WORLD); 
        MPI_Recv(&(*problemData.writeGrid)[94*rank][0], GRID_SIZE, MPI_C_BOOL, rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);     
    }


    else if(rank % 2 != 0 && rank != 15 && rank != 0){
        MPI_Recv(&(*problemData.writeGrid)[94*rank][0], GRID_SIZE, MPI_C_BOOL, rank-1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&(*problemData.writeGrid)[94*rank+1][0], GRID_SIZE, MPI_C_BOOL, rank-1, 0, MPI_COMM_WORLD);
        MPI_Recv(&(*problemData.writeGrid)[94*rank+95][0], GRID_SIZE, MPI_C_BOOL, rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&(*problemData.writeGrid)[94*rank+94][0], GRID_SIZE, MPI_C_BOOL, rank + 1, 0, MPI_COMM_WORLD);
    }

    
    else if(rank == 14){
        MPI_Send(&(*problemData.writeGrid)[94*rank+93][0], GRID_SIZE, MPI_C_BOOL, 15, 0, MPI_COMM_WORLD);
        MPI_Recv(&(*problemData.writeGrid)[94*rank+94][0], GRID_SIZE, MPI_C_BOOL, 15, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&(*problemData.writeGrid)[94*14+1][0], GRID_SIZE, MPI_C_BOOL, 13, 0, MPI_COMM_WORLD);
        MPI_Recv(&(*problemData.writeGrid)[94*14][0], GRID_SIZE, MPI_C_BOOL, 13, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    else if(rank == 15){
        MPI_Recv(&(*problemData.writeGrid)[94*15-1][0], GRID_SIZE, MPI_C_BOOL, 14, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&(*problemData.writeGrid)[94*15][0], GRID_SIZE, MPI_C_BOOL, 14, 0, MPI_COMM_WORLD);
        MPI_Recv(&(*problemData.writeGrid)[GRID_SIZE - 1][0], GRID_SIZE, MPI_C_BOOL, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&(*problemData.writeGrid)[GRID_SIZE -2 ][0], GRID_SIZE, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD);
    }
}

void evolve(ProblemData& problemData, int start, int finish) {
    auto& grid = *problemData.readGrid;
    auto& writeGrid = *problemData.writeGrid;
    local_counter=0;
    // For each cell
    for (int i = start; i < finish; i++) {
        for (int j = 1; j < GRID_SIZE - 1; j++) {
            // Calculate the number of neighbors
            int sum = grid[i - 1][j - 1] + grid[i - 1][j] + grid[i - 1][j + 1] +
                      grid[i][j - 1] + grid[i][j + 1] +
                      grid[i + 1][j - 1] + grid[i + 1][j] + grid[i + 1][j + 1];

            if (!grid[i][j]) {
                // If a cell is dead, it can start living by reproduction or stay dead
                if (sum == 3) {
                    //Count number of alive cells 
                    local_counter++;

                    // reproduction
                    writeGrid[i][j] = true;
                } else {
                    writeGrid[i][j] = false;
                }
            } else {
                // If a cell is alive, it can stay alive or die through under/overpopulation
                if (sum == 2 || sum == 3) {
                    //Count number of alive cells 
                    local_counter++;

                    // stays alive
                    writeGrid[i][j] = true;
                } else {
                    // dies due to under or overpopulation
                    writeGrid[i][j] = false;
                }
            }
        }
    }

   
}

void copy_edges(bool (&grid)[GRID_SIZE][GRID_SIZE]){

    for (int j = 1; j < GRID_SIZE - 1; j++) {
            // join columns together
            grid[0][j] = grid[GRID_SIZE - 2][j];
            grid[GRID_SIZE - 1][j] = grid[1][j];
        }
}



void copy_edges(bool (&grid)[GRID_SIZE][GRID_SIZE], int start, int finish) {

    for (int i = start - 1; i < finish ; i++) {
        // join rows together
        grid[i][0] = grid[i][GRID_SIZE - 2];
        grid[i][GRID_SIZE - 1] = grid[i][1];
    }
}

int countAlive() {
    int counter;
    MPI_Reduce(&local_counter, &counter, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    return counter;
}

void outputIntermediateSolution(int iteration, int rank, int counter) {
    if (rank == 0)
        std::cout << "Iteration " << iteration << ": " << counter << " cells alive." << std::endl;
            
}

void outputSolution() {
        std::cout << "DONE" << std::endl;
}

int main(int argc, char **argv) {

    
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    auto* problemData = new ProblemData;

    if (rank == 0)
        Utility::readProblemFromInput(CODE_VERSION, *problemData);

    MPI_Bcast(problemData->readGrid, GRID_SIZE * GRID_SIZE, MPI_C_BOOL, 0, MPI_COMM_WORLD);

    copy_edges(*problemData->readGrid);

    //Determine indexes of rows 
    positions indexes = determine_indexes(rank);

    local_counter=0;

    if (rank == 0)
        Utility::outputIntermediateSolution(0, *problemData);
            
    //TODO@Students: This is the main simulation. Parallelize it using MPI.
    for (int iteration = 0; iteration < NUM_SIMULATION_STEPS; ++iteration) {
        
        copy_edges(*problemData->readGrid, indexes.start, indexes.finish+1);

        if(iteration % SOLUTION_REPORT_INTERVAL == 0 && iteration!=0) {
            outputIntermediateSolution(iteration, rank, countAlive());
            
        }

        evolve(*problemData, indexes.start, indexes.finish);

        pass_messages(*problemData, rank);

        problemData->swapGrids();
    }

    outputIntermediateSolution(NUM_SIMULATION_STEPS, rank, countAlive());

    if(rank==0){
        outputSolution();
        }

    delete problemData;

    MPI_Finalize();
    return 0;
}