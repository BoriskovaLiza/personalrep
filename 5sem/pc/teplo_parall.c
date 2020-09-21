#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>

int main(int argc, char *argv[]){
    
	double *u, *unew, delta;
	double eps=1.e-7;
	double h, tau;
    
	int N;
	int n;
	int i, j;
    
	FILE *ff;

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
	double maxdelta, prev, futh;

	if( argc!=2 ){
		printf("Usage: exe-file N-points\n");
		exit(-1);
	}
    
	N = atoi(argv[1]);
	if (N <= 0) {
		printf("Set N to 100\n");
		N = 100;
	}

	n = (int)N/size;
	if (rank==0){
		n = N - (size-1)*(int)N/size;
	}
    
	if((u = malloc((n)*sizeof(double))) == NULL){
		printf("Can't allocate memory for u\n");
		exit(-1);
    	}

    	if((unew = malloc((n)*sizeof(double))) == NULL){
		printf("Can't allocate memory for unew\n");
		free(u);
		exit(-1);
    	}
    
    	for(i = 0; i < n; i++){
		u[i] = 0.0;
	}

	if (rank==0) {u[0] = 1.0;}

    	h = 1.0/N;
    	tau = 0.5*(h*h);

	while(1){
		maxdelta = 0.;
		delta = 0.;

		for (i = 1; i <= n - 2; i++){
			unew[i] = (tau*(u[i-1] - 2*u[i] + u[i+1])/(h*h) + u[i]);
		}

		if (rank == 0){
			MPI_Send(&u[n-1], 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
			MPI_Recv(&futh, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &status);//futh

			unew[n-1] = (tau*(u[n-2] - 2*u[n-1] + futh)/(h*h) + u[n-1]);
			unew[0] = u[0];
		}

		if (rank == size - 1){
			MPI_Recv(&prev, 1, MPI_DOUBLE, size-2, 0, MPI_COMM_WORLD, &status);//prev
			MPI_Send(&u[0], 1, MPI_DOUBLE, size-2, 0, MPI_COMM_WORLD);

			unew[0] = (tau*(prev - 2*u[0] + u[1])/(h*h) + u[0]);
			unew[n-1] = u[n-1];
		}

		if ((rank != 0) && (rank != size-1)){
			MPI_Recv(&prev, 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &status);//prev
			MPI_Send(&u[n-1], 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
			MPI_Recv(&futh, 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &status);//futh
			MPI_Send(&u[0], 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD);

			unew[0] = (tau*(prev - 2*u[0] + u[1])/(h*h) + u[0]);
			unew[n-1] = (tau*(u[n-2] - 2*u[n-1] + futh)/(h*h) + u[n-1]);
		}

		for (i = 0; i < n; i++){
			delta = fabs(unew[i]-u[i]);
			if (delta > maxdelta){ maxdelta = delta; }
		}

		st = MPI_Allreduce(&maxdelta, &maxdelta, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 4);
		}

		if (maxdelta < eps) break;

		for (i = 0; i < n; i++){
			u[i] = unew[i];
		}
    	}
    
   	if(rank == 0){
		if((ff = fopen("resfile_paral", "w+")) == NULL){
       			printf("Can't open file\n");
			free(u);
			free(unew);
       			exit(-1);
		}

		for(i = 0; i < n; ++i)
       			fprintf(ff, "%f ", unew[i]);

    		for(j = 1; j <= size - 1; ++j){
			MPI_Recv(u, (int)N/size, MPI_DOUBLE, j, 0, MPI_COMM_WORLD, &status);
			for(i = 0; i < (int)N/size; ++i)
				fprintf(ff, "%f ", u[i]);
		}
    		fclose(ff); 
	}
	else{ MPI_Send(unew, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD); }

	st = MPI_Finalize();
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 5);
	}


	free(u);
	free(unew);

    	return 0;  
}
