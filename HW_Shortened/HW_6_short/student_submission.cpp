//
// Created by Vincent Bode on 08/07/2020.
//

#include <mpi.h>
#include <cstdlib>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include "life.h"
#include "Utility.h"
#include "VideoOutput.h"

bool print = true;

#define NUM_PROC 16
int write_height = 94;
int read_height = 96;
int offset = 47;

#define CODE_VERSION 1
/*
  Apply the game of life rules on a Torus --> grid contains shadow rows and columns
  to simplify application of rules i.e. grid actually ranges from grid [ 1.. height - 2 ][ 1 .. width - 2]
*/
void evolve(ProblemData& problemData) {
    auto& grid = *problemData.readGrid;
    auto& writeGrid = *problemData.writeGrid;

    int rank;
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);
    int startIndex = write_height * (rank-1) + offset;
    int endIndex = write_height * (rank) + offset;
    if (rank == 0){
        startIndex = write_height * (NUM_PROC-1) + offset;
    }


    int i;
    int j;
    auto step = [&] () {
        int sum = grid[i - 1][j - 1] + grid[i - 1][j] + grid[i - 1][j + 1] +
                grid[i][j - 1] + grid[i][j + 1] +
                grid[i + 1][j - 1] + grid[i + 1][j] + grid[i + 1][j + 1];
        

        if (!grid[i][j]) {
            // If a cell is dead, it can start living by reproduction or stay dead
            if (sum == 3) {
                // reproduction
                writeGrid[i][j] = true;
            } else {
                writeGrid[i][j] = false;
            }
        } else {
            // If a cell is alive, it can stay alive or die through under/overpopulation
            if (sum == 2 || sum == 3) {
                // stays alive
                writeGrid[i][j] = true;
            } else {
                // dies due to under or overpopulation
                writeGrid[i][j] = false;
            }
        }
    };


    // For each cell
    if (rank != 0){
        for (i = startIndex; i < endIndex; i++) {
            for (j = 1; j < GRID_SIZE - 1; j++) {
                step();
            }
        }
    }
    else{
        for (i = startIndex; i < GRID_SIZE - 1; i++) {
            for (j = 1; j < GRID_SIZE - 1; j++) {
                step();
            }
        }
        for (i = 1; i < endIndex; i++) {
            for (j = 1; j < GRID_SIZE - 1; j++) {
                step();
            }
        }
    }
    MPI_Request req[4];
    MPI_Isend(&writeGrid[startIndex], GRID_SIZE, MPI_C_BOOL, (rank-1+16)%16, 0, MPI_COMM_WORLD, &req[0]);
    MPI_Isend(&writeGrid[endIndex-1], GRID_SIZE, MPI_C_BOOL, (rank+1)%16, 0, MPI_COMM_WORLD, &req[1]);
    MPI_Irecv(&writeGrid[endIndex],      GRID_SIZE, MPI_C_BOOL, (rank+1)%16, 0, MPI_COMM_WORLD, &req[2]);
    MPI_Irecv(&writeGrid[startIndex-1],  GRID_SIZE, MPI_C_BOOL, (rank-1+16)%16, 0, MPI_COMM_WORLD, &req[3]]);

    MPI_Waitall(4, req, MPI_STATUS_IGNORE);
}


int main(int argc, char **argv) {

    int size, rank;

    MPI_Init (NULL, NULL);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);



    if (rank==0) {
        Utility::readProblemFromInput(CODE_VERSION, *problemData);
    }
    MPI_Bcast(*problemData->readGrid, GRID_SIZE*GRID_SIZE, MPI_C_BOOL, 0, MPI_COMM_WORLD);

    //TODO@Students: This is the main simulation. Parallelize it using MPI.
    for (int iteration = 0; iteration < NUM_SIMULATION_STEPS; ++iteration) {

        if(iteration % SOLUTION_REPORT_INTERVAL == 0) {
            int startIndex;
            int endIndex;


            for (int pr=1; pr < NUM_PROC; pr++){
                startIndex = write_height * (pr-1) + offset;
                if (rank != 0 && rank==pr){
                    MPI_Send(&(*problemData->readGrid)[startIndex], GRID_SIZE*write_height, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD);
                }
                if (rank==0){
                    MPI_Recv(&(*problemData->readGrid)[startIndex], GRID_SIZE*write_height, MPI_C_BOOL, pr, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
            }

            if (rank==0){
                Utility::outputIntermediateSolution(iteration, *problemData);
            }
        }

        evolve(*problemData);

        problemData->swapGrids();
    }

    int startIndex;
    int endIndex;


    for (int pr=1; pr < NUM_PROC; pr++){
        startIndex = write_height * (pr-1) + offset;
        if (rank != 0 && rank==pr){
            MPI_Send(&(*problemData->readGrid)[startIndex], GRID_SIZE*write_height, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD);
        }
        if (rank==0){
            MPI_Recv(&(*problemData->readGrid)[startIndex], GRID_SIZE*write_height, MPI_C_BOOL, pr, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    if (rank==0){
        Utility::outputSolution(*problemData);
    }

    MPI_Finalize();
    return 0;
}

