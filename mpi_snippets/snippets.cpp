#include <stdio.h> 
#include <stdlib.h> 
#include <mpi.h>


// VALUE FROWARDING
double value;
int size, rank;
MPI_Status s;
MPI_Init (&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &size);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
value = MPI_Wtime();
printf("MPI Process %d of %d (value=%f)\n", rank, size, value);

// The order is enforced automatically, because all processes except rank 0
// have to receive from r-1 first
if (rank>0)
    MPI_Recv(&value, 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &s);
if (rank<size-1)
    MPI_Send(&value, 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD); 
if (rank==size-1)
    printf("Value from MPI Process 0: %f\n",value);
MPI_Finalize ();




// VALUE Circulating
double value;
int size, rank;
MPI_Status s;
MPI_Init (&argc, &argv);
MPI_Comm_size(MPI_COMM_WORLD, &size);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
value = MPI_Wtime();
printf("MPI Process %d of %d (value=%f)\n", rank, size, value);


if (option1){ // DEADLOCK
    MPI_Send(&value, 1, MPI_DOUBLE, (rank+1)%size, 0, MPI_COMM_WORLD);
    MPI_Recv(&value, 1, MPI_DOUBLE, (rank-1+size)%size, 0, MPI_COMM_WORLD, &s);
}
if (option2){
    // SendRecv
    MPI_Sendrecv(
        &value, 1, MPI_DOUBLE, (rank+1)%size, 0,     
        &value, 1, MPI_DOUBLE, (rank-1+size)%size, 0,
        MPI_COMM_WORLD, &s);
}
if (option3){
// SendRecv same buffer
    MPI_Sendrecv_replace(
        &value, 1, MPI_DOUBLE, 
        (rank+1)%size, 0,
        (rank-1+size)%size, 0, 
        MPI_COMM_WORLD, &s);
}
if (option4){
    MPI_Request req[2];
    MPI_Status status[2];
    MPI_Isend(&value, 1, MPI_DOUBLE, (rank+1)%size, 0, MPI_COMM_WORLD, &(req[0]);
    MPI_Irecv(&value, 1, MPI_DOUBLE, (rank-1+size)%size, 0, MPI_COMM_WORLD, &(req[1]);
    MPI_Waitall(2, req, status);
}
MPI_Finalize ();


int msg_in[10], 
msg_out[10];
MPI_Isend(msg_out, 10, MPI_INT, 1, 42, MPI_COMM_WORLD, &(req[0]); 
MPI_Irecv(msg_in,  10, MPI_INT, 1, 42, MPI_COMM_WORLD, &(req[1]);
MPI_Waitall(2, req, status);


// SEPARATE BY CHUNKS
// Broadcast Angaben, process queries on each chunk separately, Reduce results
MPI_Bcast(queries, LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);

int occurrences_list[NUM_QUERIES] = {0};  // Each process wokrs on it's chunk of document and on NUM_QUERIES
process(document, rank, occurrences_list);

int result[NUM_QUERIES];
MPI_Reduce(occurrences_list, result, NUM_QUERIES, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

// Print
if (rank==0){
    for (int queryId = 0; queryId < NUM_QUERIES; queryId++) {
        printf("Query %i: %i occurrences (query length %zu).\n",
                queryId, result[queryId]);
    }
}

// SEPARATE BY QUERIES
// Broadcast Angaben, process queries on each chunk separately, Reduce results
MPI_Bcast(queries, LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);

int occurrences_scalar = 0;  // Each process wokrs on it's chunk of document and on NUM_QUERIES
process(document, rank, occurrences_scalar);

int result[NUM_QUERIES];
MPI_Reduce(occurrences_scalar, result, NUM_QUERIES, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

// Print
if (rank==0){
    for (int queryId = 0; queryId < NUM_QUERIES; queryId++) {
        printf("Query %i: %i occurrences (query length %zu).\n",
                queryId, result[queryId]);
    }
}