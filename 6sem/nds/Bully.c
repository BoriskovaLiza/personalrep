#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include <unistd.h>
#include <signal.h>

//tags: voting=1; ok=2; coordinator=3

const int RANK = 1;
const unsigned int timeout = 5;

int main(int argc, char *argv[]){

	int i = 0;
    	int sigflag = 1;
	int msgflag = 0;

	void handler(int nsig){
		sigflag = 0;
	}
	
	signal(SIGALRM, handler);

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

	srandom(rank);
	if ((rank != RANK) && (random()%2 == 1)){
		printf("Process #%d randomly stopped working\n", rank);
		st = MPI_Finalize();
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 4);
		}
		return 0;
	}

	if (rank == RANK){
		printf("Process #%d started voting:\n", rank);
		for (i = rank + 1; i < size; i++){
			st = MPI_Send(&rank, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 5);
			}
			printf("	sending \"vote\" to #%d\n", i);	
		}
		alarm(timeout);
		while(sigflag){
			st = MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &msgflag, &status);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 6);
			}
			if (msgflag && status.MPI_TAG == 2){
				st = MPI_Recv(&i, 1, MPI_INT, MPI_ANY_SOURCE, 2, MPI_COMM_WORLD, &status);
				alarm(0);
				printf("Process #%d got \"ok\" from %d and waits the end of election\n", rank, status.MPI_SOURCE);
				st = MPI_Recv(&i, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
				printf("Process #%d got that a coordinator is now #%d and stopped working\n", rank, status.MPI_SOURCE);
				st = MPI_Finalize();
				if (st!=MPI_SUCCESS){
					MPI_Abort(MPI_COMM_WORLD, 7);
				}
				return 0;
			}
			if (msgflag && status.MPI_TAG == 1){
				printf("Process #%d got \"vote\" from %d and will send back \"ok\"\n", rank, status.MPI_SOURCE);
				st = MPI_Send(&i, 1, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD);
			}
		}
		printf("Process #%d didn't recieve any \"ok\" and became a coordinator\n", rank);
		for (i = 0; i < size; i++){
			if (rank != RANK){
				st = MPI_Send(&rank, 1, MPI_INT, i, 3, MPI_COMM_WORLD);
				if (st!=MPI_SUCCESS){
					MPI_Abort(MPI_COMM_WORLD, 8);
				}
				printf("	sending \"coord\" to #%d\n", i);
			}
		}
		printf("Process #%d stopped working as a coordinator\n", rank);
		st = MPI_Finalize();
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 9);
		}
		return 0;
	}

	else{
		st = MPI_Recv(&i, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if (status.MPI_TAG == 3){
			printf("Process #%d got that a coordinator is now #%d and stopped working\n", rank, status.MPI_SOURCE);
				st = MPI_Finalize();
				if (st!=MPI_SUCCESS){
					MPI_Abort(MPI_COMM_WORLD, 14);
				}
				return 0;
		}
		printf("Process #%d got \"vote\" from %d and will send back \"ok\"\n", rank, status.MPI_SOURCE);
		st = MPI_Send(&i, 1, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 11);
		}


		printf("Process #%d started voting:\n", rank);
		for (i = rank + 1; i < size; i++){
			st = MPI_Send(&rank, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 12);
			}
			printf("	sending \"vote\" to #%d\n", i);	
		}
		alarm(timeout);
		while(sigflag){
			st = MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &msgflag, &status);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 13);
			}
			if (msgflag){
					st = MPI_Recv(&i, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
					if (status.MPI_TAG == 1){
						printf("Process #%d got \"vote\" from %d and will send back \"ok\"\n", rank, status.MPI_SOURCE);						
						st = MPI_Send(&i, 1, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD);
						if (st!=MPI_SUCCESS){
							MPI_Abort(MPI_COMM_WORLD, 11);
						}
					}
					if (status.MPI_TAG == 2){
						printf("Process #%d got \"ok\" from %d and waits the end of election\n", rank, status.MPI_SOURCE);
						alarm(0);
					}
					if (status.MPI_TAG == 3){
						printf("Process #%d got that a coordinator is now #%d and stopped working\n", rank, status.MPI_SOURCE);
						st = MPI_Finalize();
						if (st!=MPI_SUCCESS){
							MPI_Abort(MPI_COMM_WORLD, 14);
						}
						return 0;
					}
			}
		}
		printf("Process #%d didn't recieve any \"ok\" and became a coordinator\n", rank);
		for (i = 0; i < size; i++){
			if (rank != RANK){
				st = MPI_Send(&rank, 1, MPI_INT, i, 3, MPI_COMM_WORLD);
				if (st!=MPI_SUCCESS){
					MPI_Abort(MPI_COMM_WORLD, 15);
				}
				printf("	sending \"coord\" to #%d\n", i);
			}
		}
		printf("Process #%d stopped working as a coordinator\n", rank);
		st = MPI_Finalize();
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 16);
		}
		return 0;
	}

	st = MPI_Finalize();
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 17);
	}

    	return 0;  
}
