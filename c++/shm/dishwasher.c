#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <unistd.h>
#include <stdlib.h>

#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#include <errno.h>

int search(FILE *file, char* dish_type){
		int delay = 0;
		char name[2048];
		rewind(file);
		while(fscanf(file, "%s %d", name, &delay)!=EOF){
			if (strcmp(name, dish_type) == 0) return delay;
		}
		printf("Dishwasher: unknown type of tableware: %s\n", dish_type);
		return -1;//ненашли посудку
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

	int number = 0;
	int point = 0;
	int i = 0;
	char dish_t[256];

	FILE *input, *gost; 
	char name[2048]; 
	input = fopen ("queue", "r");
	gost = fopen ("gost_washing", "r");

	k = ftok("queue", 1);
	int table = semget(k, 1, IPC_CREAT|0664);//для количества посуды на столе
	perror("table built");

	k = ftok("queue", 2);
	int duty = semget(k, 1, IPC_CREAT|0664);//для конца смены
	perror("duty started");

	struct sembuf wait_empty = {0, 0, 0};
	struct sembuf fill = {0, 2, 0}, unfill = {0, -2, 0};
 
	while (fscanf(input, "%s %d", dish_t, &number) == 2){
		//обязательно написать EOF как eof в файле!
		int delay = search(gost, dish_t);
		printf("Got: %s %d\n", dish_t, number);
		for (i = 0; i < number; i++){
			sleep(delay);
			if (point == TABLE_LIMIT){
				semop(table, &fill, 1);
				point = point - TABLE_LIMIT;
				semop(table, &wait_empty, 1);
			}
			strcpy(place[point].type, dish_t);
			printf("Washed: %s\n", dish_t);
			point++;
		}
	}
	
	if (point < TABLE_LIMIT){
		strcpy(place[point].type, "end_of_dishes");
	}
	semop(table, &fill, 1);
	semop(table, &wait_empty, 1);

	semop(duty, &unfill, 1);
	int p = shmdt(mem);
	perror("dettach:");

	return 0;
}
