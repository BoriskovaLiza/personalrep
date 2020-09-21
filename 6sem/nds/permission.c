#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include <unistd.h>
#include <signal.h>

//tags: request = 1; permit = 2; release = 3

int REPEAT = 3;
int sigflag = 1;

void handler(int nsig){
	sigflag = 0;
}

int main(int argc, char *argv[]){

	signal(SIGALRM, handler);
	int i = 0;
	int msgflag = 0;

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

	if (rank == 0){
		int operations = (size-1)*REPEAT;
		while (operations > 0){
			st = MPI_Recv(&i, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 4);
			}
			printf("Got a request from #%d\n", status.MPI_SOURCE);
			st = MPI_Send(&i, 1, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 5);
			}
			printf("Sent permission and now waiting #%d to end critical section\n", status.MPI_SOURCE);
			st = MPI_Recv(&i, 1, MPI_INT, status.MPI_SOURCE, 3, MPI_COMM_WORLD, &status);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 6);
			}
			printf("#%d finished\n", status.MPI_SOURCE);
			operations--;
		}
	}
	
	else{
		while(REPEAT){
			REPEAT--;
			//entry
			printf("	#%d requesting critical section\n", rank);
			st = MPI_Send(&i, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 7);
			}
			st = MPI_Recv(&i, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 8);
			}
			printf("	#%d got permission\n", rank);
			//critical section
			sleep(rand()%5+1);
			//exit
			printf("	#%d completed critical job and sends release\n", rank);
			st = MPI_Send(&i, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 9);
			}
			//remainder section
			sleep(rand()%5+1);
			printf("	#%d completed all jobs with %d remaining\n", rank, REPEAT);
		}
	}

	st = MPI_Finalize();
	if (st!=MPI_SUCCESS){
		MPI_Abort(MPI_COMM_WORLD, 10);
	}
	return 0;	
}

