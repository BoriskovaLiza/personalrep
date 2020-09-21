#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]){

	const int d = 1;
	int k;
	int* V;
	int* Vmsg;

	MPI_Status status;
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

	rank++;
	char filename[10];
	sprintf(filename, "v0%d.dat", rank);

	FILE *dat;
	dat = fopen(filename, "r");
	if (dat == NULL){
		printf("Process %d has no job to do.\n", rank);
		st = MPI_Finalize();
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 6);
		}
		return 0;
	}

	if((V = (int*)malloc((size)*sizeof(int))) == NULL){
		printf("Can't allocate memory for V\n");
		exit(-1);
    	}

	if((Vmsg = (int*)malloc((size)*sizeof(int))) == NULL){
		printf("Can't allocate memory for V\n");
		exit(-1);
    	}

	for (int i = 0; i < size; i++){
		V[i] = 0;
		Vmsg[i] = 0;
	}


	while(fscanf(dat, "%d", &k) != EOF){
		V[rank-1] += d;

		if (k == 0){
			printf("#%d performed one task.\n", rank);

			printf("	#%d time is V = ( ", rank);
			for (int i = 0; i < size; i++) printf("%d ", V[i]);
			printf(")\n");
		}
		else if (k < 0){
			printf("#%d is recieving time of #%d.\n", rank, -k);

			printf("	#%d time is V = ( ", rank);
			for (int i = 0; i < size; i++) printf("%d ", V[i]);
			printf(")\n");

			st = MPI_Recv(Vmsg, size, MPI_UNSIGNED, -k-1, 0, MPI_COMM_WORLD, &status);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 5);
			}

			for (int i = 0; i < size; i++){
				if (i != rank-1){
					V[i] = std::max(Vmsg[i], V[i]);
				}
			}

			printf("	#%d corrected time to V = ( ", rank);
			for (int i = 0; i < size; i++) printf("%d ", V[i]);
			printf(")\n");
		}
		else if (k > 0){
			printf("#%d is sending it's time to #%d.\n", rank, k);

			printf("	#%d time is V = ( ", rank);
			for (int i = 0; i < size; i++){
				printf("%d ", V[i]);
			}
			printf(")\n");

			for (int i = 0; i < size; i++) Vmsg[i] = V[i];

			st = MPI_Send(Vmsg, size, MPI_UNSIGNED, k-1, 0, MPI_COMM_WORLD);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 4);
			}
		}	
	}

	fclose(dat);

	st = MPI_Finalize();
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 6);
	}

    	return 0;  
}
