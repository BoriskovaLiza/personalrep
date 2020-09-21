#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
int main(){

	key_t k;
	int semid;
	struct sembuf ops[1];
	perror("2_sembuf");
	ops[0].sem_num = 0;
	ops[0].sem_op = 1;
	ops[0].sem_flg = 0;
	semop(semid, ops, 1);
	perror("2_continue");
//	sleep(5);
	printf("%s", "15_2\n");
	return 0;
}
