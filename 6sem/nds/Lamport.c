#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]){
    
	unsigned int L = 0, Lmsg = 0;
	const int d = 1;
	int k;	

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
	sprintf(filename, "0%d.dat", rank);

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

	while(fscanf(dat, "%d", &k) != EOF){
		L += d;
		if (k == 0){
			printf("#%d performed one task.\n", rank);
			printf("	#%d L = %d.\n", rank, L);
		}
		else if (k < 0){
			printf("#%d is recieving time of #%d.\n", rank, -k);
			printf("	#%d L = %d.\n", rank, L);

			st = MPI_Recv(&Lmsg, 1, MPI_UNSIGNED, -k-1, 0, MPI_COMM_WORLD, &status);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 5);
			}
			L = std::max(L, Lmsg + d);
			printf("	#%d corrected time to L = %d.\n", rank, L);
		}
		else if (k > 0){
			printf("#%d is sending it's time to #%d.\n", rank, k);
			printf("	#%d L = %d.\n", rank, L);

			Lmsg = L;
			st = MPI_Send(&Lmsg, 1, MPI_UNSIGNED, k-1, 0, MPI_COMM_WORLD);
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
