#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <wait.h>

int search(FILE *file, char* dish_type){
		int delay = 0;
		char name[2048];
		rewind(file);
		while(fscanf(file, "%s %d", name, &delay)!=EOF){
			if (strcmp(name, dish_type) == 0) return delay;
		}
		return -1;//не нашли посудку
};


struct dish{
        char type[256];
};

int main(){

	char *var = getenv ("TABLE_LIMIT");
	if (var == NULL)
	{
		printf ("TABLE_LIMIT not found\n");
		exit (0);
	}

	char *end;
	const int TABLE_LIMIT = strtol (var, &end, 10);
	printf("Current limit: %d\n", TABLE_LIMIT);

	key_t k = ftok("numb", 4);
	int shmid = shmget(k, TABLE_LIMIT*sizeof(struct dish), IPC_CREAT|0664);
	if (shmid < 0) {perror("shmid"); return -1;}

	void* mem = shmat(shmid, NULL, 0);
	struct dish* place = (struct dish*)mem;	

	FILE *input, *gost; 
	char name[2048]; 
	input = fopen ("queue", "r");
	gost = fopen ("gost_drying", "r");

	k = ftok("queue", 1);
	int table = semget(k, 1, IPC_CREAT|0664);//для количества посуды на столе
	perror("table built");

	k = ftok("queue", 2);
	int duty = semget(k, 1, IPC_CREAT|0664);//для конца смены
	perror("duty started");

	struct sembuf is_on = {0, 0, IPC_NOWAIT}, wait_full = {0, -1, 0};
	struct sembuf fill = {0, 2, 0}, unfill = {0, -1, 0};
	semop(duty, &fill, 1);

	while(semop(duty, &is_on, 1) != 0){
		semop(table, &wait_full, 1);
		for(int i = 0; i < TABLE_LIMIT; i++){
			int delay = search(gost, place[i].type);
			if (delay < 0){break;}
			printf("Got: %s\n", place[i].type);
			sleep(delay);
			printf("Dryed: %s\n", place[i].type);
		}
		semop(table, &unfill, 1);
		printf("Waiting filled table\n");
		sleep(2);
	}

	int p = shmdt(mem);
	perror("dettach:");

	return 0;
}
