#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include <unistd.h>
#include <signal.h>

//tags: voting=1; ok=2; coordinator=3

const int RANK = 1;
const unsigned int timeout = 2;

int sigflag = 1;

void handler(int nsig){
	sigflag = 0;
}

int main(int argc, char *argv[]){

	int* cand;//if processes are candidates or not

	int i = 0;
	int coord;
	int msgflag = 0;
	
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

	if((cand = (int*)malloc((size)*sizeof(int))) == NULL){
		printf("Can't allocate memory for cand\n");
		exit(-1);
    	}
	
	for (i = 0; i < size; i++){
		cand[i] = 0;
	}
    	
	MPI_Status status;
	srandom(rank);
	int next = (rank + 1)%size;

	if ((rank != RANK) && (random()%2 == 1)){
		printf("Process #%d randomly stopped working\n", rank);
		st = MPI_Finalize();
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 4);
		}
		return 0;
	}

	if (rank == RANK){
		printf("Process #%d started elections:\n", rank);
		cand[rank] = 1;

		for (next; next != rank;){
			printf("	sending candidates list to #%d\n", next);
			st = MPI_Send(cand, size, MPI_INT, next, 1, MPI_COMM_WORLD); //!!!
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 5);
			}
			sigflag = 1;
			alarm(timeout);
			while(sigflag){
				st = MPI_Iprobe(next, 2, MPI_COMM_WORLD, &msgflag, &status);
				if (st!=MPI_SUCCESS){
					MPI_Abort(MPI_COMM_WORLD, 6);
				}
				if (msgflag){
					st = MPI_Recv(cand, size, MPI_INT, next, 2, MPI_COMM_WORLD, &status);
					if (st!=MPI_SUCCESS){
						MPI_Abort(MPI_COMM_WORLD, 7);
					}
					alarm(0);
					printf("	next to %d is %d\n", rank, next);
					break;
				}
			}
			if (msgflag) { break; }
			printf("Didn't get anything from #%d\n", next);
			next = (next+1)%size;
		}
		if (next == rank){
			printf("I'm alone here\n");
			st = MPI_Finalize();
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 7);
			}
			return 0;	
		}
		printf("Process #%d waiting for the circle to end\n", rank);
		st = MPI_Recv(cand, size, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 8);
		}
		printf("Circle ended\n", rank);
		st = MPI_Send(cand, size, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 8);
		}
		if (cand[rank]==1) {
			printf("...searching for a coordinator in %d\n", rank);
			for (i = 0; i < size; i++){
				if (cand[i] == 1){
					coord = i;
				}
			}
			printf("   coordinator is now %d\n", coord);
			st = MPI_Send(&coord, 1, MPI_INT, next, 3, MPI_COMM_WORLD);
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 8);
			}
		}
		st = MPI_Recv(&coord, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 8);
		}
		printf("Process #%d got that a coordinator is now #%d and stopped working\n", rank, coord);
		st = MPI_Finalize();
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 16);
		}
		return 0;
	}



	else{
		st = MPI_Recv(cand, size, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);	
		printf("Process #%d got candidates list from #%d and will also compete\n", rank, status.MPI_SOURCE);
		cand[rank] += 1;
		printf("	#%d sending back \"ok\" #%d\n", rank, status.MPI_SOURCE);
		st = MPI_Send(cand, size, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 8);
		}

		for (next; next != rank;){
			printf("	#%d -> #%d\n", rank, next);
			st = MPI_Send(cand, size, MPI_INT, next, 1, MPI_COMM_WORLD); //!!!
			if (st!=MPI_SUCCESS){
				MPI_Abort(MPI_COMM_WORLD, 5);
			}
			sigflag = 1;
			alarm(timeout);
			while(sigflag){
				st = MPI_Iprobe(next, 2, MPI_COMM_WORLD, &msgflag, &status);
				if (st!=MPI_SUCCESS){
					MPI_Abort(MPI_COMM_WORLD, 6);
				}
				if (msgflag){
					st = MPI_Recv(cand, size, MPI_INT, next, 2, MPI_COMM_WORLD, &status);
					if (st!=MPI_SUCCESS){
						MPI_Abort(MPI_COMM_WORLD, 7);
					}
					alarm(0);
					printf("	next to %d is %d\n", rank, next);
					break;
				}
			}
			if (msgflag) { break; }
			printf("	#%d -/-> #%d\n", rank, next);
			next = (next+1)%size;
		}

		st = MPI_Recv(&coord, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 8);
		}
		st = MPI_Send(&coord, 1, MPI_INT, next, 3, MPI_COMM_WORLD);
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 8);
		}
		printf("Process #%d got that a coordinator is now #%d and stopped working\n", rank, coord);
		st = MPI_Finalize();
		if (st!=MPI_SUCCESS){
			MPI_Abort(MPI_COMM_WORLD, 16);
		}
		return 0;	
	
	}
}
