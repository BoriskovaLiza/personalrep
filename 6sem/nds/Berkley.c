#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>

int main(int argc, char *argv[]){
    
	unsigned long int time0 = 0;
	unsigned long int time1 = 0;
	int total_diff = 0;
	int* diff;
	int loc_diff = 0;
	const int k = 50;	

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
	srandom(rank+2);
	time0 += random()%k;

	if((diff = (int*)malloc((size)*sizeof(int))) == NULL){
		printf("Can't allocate memory for diff\n");
		exit(-1);
    	}
	diff[0] = 0;
    
	if (rank == 0){
		printf("Time server's time is %d\n", time0);
		time1 = time0;
	}
	else {
		printf("#%d time is %d\n", rank, time0);
	}

	MPI_Bcast(&time1, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

	if (rank==0){
		for (int i = 1; i <= size - 1; i++){
			MPI_Recv(&loc_diff, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			printf("From %d got diff %d\n", status.MPI_SOURCE, loc_diff);
			diff[status.MPI_SOURCE] = loc_diff;
			total_diff += loc_diff;
		}
		total_diff = ceil(total_diff/size);
		printf("TOTAL DIFF %d\n", total_diff);
		time1 += total_diff;
		printf("#0 fixed time to %d\n", time1);
		for (int i = 1; i <= size - 1; i++){
			diff[i] = total_diff - diff[i];
			MPI_Send(&diff[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}
	else{
		loc_diff = time0 - time1;
		MPI_Send(&loc_diff, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		printf("#%d sent his time difference %d\n", rank, loc_diff);
		MPI_Recv(&loc_diff, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		time1 = time0 + loc_diff;
		printf("#%d fixed time to %d\n", rank, time1);
	}

	st = MPI_Finalize();
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 4);
	}

	free(diff);

    	return 0;  
}
