#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <mpi.h>

void Gorner(int i, const double *x, double *F, const double B[], const double X[]){
	if (i == -1) { return; }
	*F = *F * (*x - X[i]) + B[i];
	Gorner(i-1, x, F, B, X);
	return; 
}

//parallel version w/o breckets solving

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

	if (argc < 2) {
		if (rank == 0) { printf("Please, write input and output filenames.\n"); }
		st = MPI_Finalize();
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 4);
		}
	}

	int N = 0; //Number of points
	int i = 0;
	int j = 0;
	double *X, *Y, *B;
	double x, F = 0.; //The point to be interpolated

	char* filename;
	FILE* points;
	FILE* output;
	MPI_Status status;
	time_t start, end;

	start = time (&start);

	filename = argv[1];
	if ((points = fopen(filename, "r"))==NULL) {
		if (rank == 0) { printf("Cannot open input file.\n"); }
		MPI_Abort(MPI_COMM_WORLD, 6);
	}

	filename = argv[2];
	if ((output = fopen(filename, "w"))==NULL) {
		if (rank == 0) { printf("Cannot open output file.\n"); }
		MPI_Abort(MPI_COMM_WORLD, 6);
	}

	//Reading number of points
	fscanf(points, "%d", &N);

	if((X = (double*)malloc((N)*sizeof(double))) == NULL){
		printf("Can't allocate memory for X\n");
		exit(-1);
    	}
	if((Y = (double*)malloc((N)*sizeof(double))) == NULL){
		printf("Can't allocate memory for Y\n");
		exit(-1);
    	}
	if((B = (double*)malloc((N)*sizeof(double))) == NULL){
		printf("Can't allocate memory for B\n");
		exit(-1);
    	}

	//Reading initial points
	for(; i < N; i++){ fscanf(points, "%le %le", &X[i], &Y[i]); }

	fscanf(points, "%le", &x);
	fclose(points);

	B[0] = Y[0];

	for (int j = 1; j < N; j++){
		if (N-j < size) {size--;}
		if (N-j < rank + 1){
			st = MPI_Finalize();
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 4);
			}
			return 0;
		}

		//Computing responsibility area
		int first = (int)rank*(N-j)/size;
		int last = (int)(rank+1)*(N-j)/size - 1;

		//Computing separated differences
		for (i = first; i <= last; i++){
			B[i+j] = (Y[j+i]-Y[j+i-1])/(X[i+j]-X[i]);
		}

		for (i = first; i <= last; i++){
			Y[i+j] = B[i+j];
		}

		//Exchange of edges, nessesary for the next step
		if ((rank == 0) && (size > 1)){
			MPI_Send(&Y[j+last], 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
			MPI_Recv(&Y[j+last+1], 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);
		}

		if ((rank == size - 1) && (size > 1)){
			MPI_Recv(&Y[j+first-1], 1, MPI_DOUBLE, size-2, 0, MPI_COMM_WORLD, &status);
			MPI_Send(&Y[j+first], 1, MPI_DOUBLE, size-2, 0, MPI_COMM_WORLD);
		}

		if ((rank != 0) && (rank != size-1)){
			MPI_Recv(&Y[j+first-1], 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &status);
			MPI_Send(&Y[j+last], 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
			MPI_Recv(&Y[j+last+1], 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &status);
			MPI_Send(&Y[j+first], 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);
		}
	}

	//Computing interpolated F(x) recursively
	F = B[N-1];
	Gorner(N-2, &x, &F, B, X);
	fprintf(output, "%le %le\n", x, F);

	//Computing worktime
	end = time (&end);
	fprintf(output, "%.0e\n", difftime(end, start));

	st = MPI_Finalize();
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 4);
	}

	free(B);
	free(Y);
	free(X);
	fclose(output);

	return 0;
}
