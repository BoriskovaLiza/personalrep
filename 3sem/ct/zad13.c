#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/sem.h>
#include <unistd.h>

int N[3] = {0, 0, 0};
int semid;
 struct sembuf ops1 = {0, 1, 0}, ops2 = {0, -1, 0};


void* my_thread_func(void* dummy){
	int i = 0;
	for(; i < 1000000; ++i){
		semop(semid, &ops2, 1);
		N[0] += 1;
		semop(semid, &ops1, 1);
	}
}

int main(){

	key_t k = ftok("txt1", 0);
        perror("ftok");
        semid = semget(k, 1, IPC_CREAT|0664);
        perror("semget");
	semop(semid, &ops1, 1);

	pthread_t thr;
	int r = pthread_create(&thr, NULL, my_thread_func, NULL);
	if (r!=0) {printf("@ %d\n", r);}

	my_thread_func(NULL);
	pthread_join(thr, NULL);


	printf("# %d\n", N[0]);

	return 0;
}

