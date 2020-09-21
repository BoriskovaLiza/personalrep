//computes pi
//compile with "mpicc -Wall -lm -o math pr_4.c -o a_4.out"
#include <mpi.h>
#include <math.h>
#include <stdio.h>

#define N 1000000

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

	double h = 2.0/N;
	double sum = 0;
	double x = 0;

	if (rank == 0){
		double tmp = 0;
		int st, i;
		MPI_Status status;

		for (i = 0; i <= N/size - 1; i++){
			sum += 0.5*(sqrt(4.0 - x*x) + sqrt(4.0-(x+h)*(x+h)))*h;
			x = x + h;
		}

		for (i = 0; i < size-1; i++){		
			st = MPI_Recv(&tmp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 5);
			}
			sum += tmp;
		}
		printf("-------\npi = %f\n-------\n", sum);
	}
	else {
		double tmp = 0;
		int st, i;

		x = x + h*rank*N/size;

		for (i = 0; i <= N/size - 1; i++){
			tmp += 0.5*(sqrt(4.0 - x*x) + sqrt(4.0-(x+h)*(x+h)))*h;
			x = x + h;
			}

		printf("from %d got %f\n", rank, tmp);

		st = MPI_Send(&tmp, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 4);
		}
	}
	

	st = MPI_Finalize();
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 6);
	}
	return 0;
}
