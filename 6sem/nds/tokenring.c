#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include <unistd.h>
#include <signal.h>

//tags: marker = 1, FIN = 2

int REPEAT = 3;
int sigflag = 1;

void handler(int nsig){
	sigflag = 0;
}

int main(int argc, char *argv[]){

	signal(SIGALRM, handler);
	int i = 0;

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
	srand(rank);

	int next = (rank + 1)%size;
	int prev = (rank - 1)%size;
	int msgflag = 0;

	if (rank == 0){
		printf("#%d created a marker\n", rank);
		st = MPI_Send(&i, 1, MPI_INT, next, 1, MPI_COMM_WORLD);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 4);
		}
	}
		

	while(REPEAT){
		REPEAT--;
		printf("	#%d is ready for critical section\n", rank);
		//entry
		if (!msgflag){
			st = MPI_Recv(&i, 1, MPI_INT, prev, 1, MPI_COMM_WORLD, &status);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 5);
			}
			printf("#%d got a marker\n", rank);
		}
		//critical section
		sleep(rand()%2+1);
		printf("	#%d did his critical section with %d remaining\n", rank, REPEAT);
		//exit
		st = MPI_Send(&i, 1, MPI_INT, next, 1, MPI_COMM_WORLD);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 6);
		}
		printf("marker #%d -> #%d", rank, next);
		//remainder section
		alarm(rand()%5+1);
		printf("	#%d now does his remainder job\n", rank, REPEAT);
		while(sigflag){
			if (!msgflag){
				st = MPI_Iprobe(prev, 1, MPI_COMM_WORLD, &msgflag, &status);
				if (st!=MPI_SUCCESS){
					MPI_Abort(MPI_COMM_WORLD, 7);
				}
				if (msgflag){
					st = MPI_Recv(&i, 1, MPI_INT, prev, 1, MPI_COMM_WORLD, &status);
					if (st!=MPI_SUCCESS){
						MPI_Abort(MPI_COMM_WORLD, 8);
					}
					printf("#%d got a marker while remainder section\n", rank);
				}
			}
		}
	}
	//finalizing
	printf("#%d completed his jobs\n", rank);
	if (rank == 0){
		printf("#%d created a FIN message\n", rank);
		st = MPI_Send(&i, 1, MPI_INT, next, 2, MPI_COMM_WORLD);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 9);
		}
	}
	else {
		msgflag = 0;
		while (!msgflag){
			st = MPI_Recv(&i, 1, MPI_INT, prev, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 10);
			}
			if (status.MPI_TAG == 1){
				st = MPI_Send(&i, 1, MPI_INT, next, 1, MPI_COMM_WORLD);
				if (st!=MPI_SUCCESS){
					MPI_Abort(MPI_COMM_WORLD, 11);
				}
				msgflag = 0;
				printf("marker #%d -> #%d\n", rank, next);
			}
			else{
				printf("	#%d passing a FIN message\n", rank);
				st = MPI_Send(&i, 1, MPI_INT, next, 2, MPI_COMM_WORLD);
				if (st!=MPI_SUCCESS){
					MPI_Abort(MPI_COMM_WORLD, 11);
				}
				msgflag = 1;
			}
		}
	}
	//everyone passed FIN once so that marker stopped
	msgflag = 0;
	while (!msgflag){
		st = MPI_Recv(&i, 1, MPI_INT, prev, 2, MPI_COMM_WORLD, &status);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 10);
		}
		printf("#%d got a FIN message secondly and exited\n", rank);
		if (rank != size-1){
			st = MPI_Send(&i, 1, MPI_INT, next, 2, MPI_COMM_WORLD);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 11);
			}
		}
		st = MPI_Finalize();
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 12);
		}
		return 0;
	}
}
