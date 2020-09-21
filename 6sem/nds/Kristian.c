#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>

int main(int argc, char *argv[]){
    
	unsigned long int time0 = 0;
	unsigned long int time1 = 0;
	unsigned long int UTC = 0;
	const int k = 40;	

	int st;
	st = MPI_Init(&argc, &argv);
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 1);
	}

	int size = 0;
	st = MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 2);
	}
	int rank = 0;
	st = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 3);
	}
    	
	MPI_Status status;
	srandom(rank+1);
    
	if (rank == 0){
		for (int i = 1; i <= size - 1; i++){
			UTC += random()%k;
			MPI_Recv(&time0, 1, MPI_UNSIGNED_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			printf("UTC is %d, talking to %d\n", UTC, status.MPI_SOURCE);
			MPI_Send(&UTC, 1, MPI_UNSIGNED_LONG, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
		}
	}
	else if (rank > 0){
		time0 += random()%k;
		printf("#%d time is %d\n", rank, time0);
		MPI_Send(&time0, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD);
		time1 = time0 + random()%k;
		printf("#%d sent his time, now it's %d\n", rank, time1);
		MPI_Recv(&UTC, 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD, &status);
		time1 = UTC + ceil((time1 - time0)/2);
		printf("#%d fixed time to %d\n", rank, time1);
	}

	st = MPI_Finalize();
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 4);
	}

    	return 0;  
}
