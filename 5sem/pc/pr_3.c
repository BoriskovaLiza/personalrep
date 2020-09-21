#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]){
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

	if (rank == 0){
		int p = 0;
		int st;
		MPI_Status status;
		printf("from 0 sending %d\n", p);
		st = MPI_Send(&p, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 4);
		}
		st = MPI_Recv(&p, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 7);
		}
		printf("%d\n", p);
	}
	else {
		int p = 0;
		int st;
		MPI_Status status;
		st = MPI_Recv(&p, 1, MPI_INT, rank-1, 1, MPI_COMM_WORLD, &status);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 5);
		}
		p += rank;
		printf("from %d sending %d\n", rank, p);
		if (rank != size-1){
			st = MPI_Send(&p, 1, MPI_INT, rank+1, 1, MPI_COMM_WORLD);
		}
		else{
			st = MPI_Send(&p, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		}
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 6);
		}
	}

	st = MPI_Finalize();
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 8);
	}
	return 0;
}
