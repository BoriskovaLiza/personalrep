#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdio.h>
int main(){

	key_t k = ftok("txt1", 0);
	perror("1_ftok");
	int semid = semget(k, 1, IPC_CREAT|0664);
	perror("1_semget");
	struct sembuf ops[1];
	ops[0].sem_num = 0;
	ops[0].sem_op = -5;
	ops[0].sem_flg = 0;
	semop(semid, ops, 1);
	perror("1_continue");
        printf("%s", "15_1\n");

	return 0;
}
