//computes pi
//compile with "mpicc -Wall -lm -o math pr_4.c -o a_4.out"
#include <mpi.h>
#include <math.h>
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

	FILE *file;
	file = fopen("N", "r");

	int N = 100000;

	if (rank == 0){
		fscanf(file, "%d", &N);
	}

	st = MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 4);
	}

	double h = 2.0/N;
	double sum = 0;
	double x = 0;
	int i = 0;

	double tmp = 0;

	x = x + h*rank*N/size;

	for (i = 0; i <= N/size - 1; i++){
		tmp += 0.5*(sqrt(4.0 - x*x) + sqrt(4.0-(x+h)*(x+h)))*h;
		x = x + h;
		}

	printf("from %d got %f\n", rank, tmp);

	st = MPI_Reduce(&tmp, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 5);
	}
	
	if (rank == 0){ printf("Total %f\n", sum); }

	st = MPI_Finalize();
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 6);
	}

	fclose(file);

	return 0;
}
