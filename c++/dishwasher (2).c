#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>

#include <fcntl.h>
#include <stdio.h>

#include <errno.h>
#include <wait.h>

const int TABLE_LIMIT = 7;

int search(FILE *file, char* dish_type){
		int delay = 0;
		char name[2048];
		rewind(file);
		while(fscanf(file, "%s %d", name, &delay)!=EOF){
			if (strcmp(name, dish_type) == 0) return delay;
		}
		printf("Dishwasher: unknown type of tableware: %s", dish_type);
		sleep(3);
		return -1;//ненашли посудку
};

int main(){

	int washed_dishes = 0;
	int number = 0;
	
	key_t k = ftok("queue", 1);
	int semid = semget(k, 1, IPC_CREAT|0664);//для количества посуды на столе
	perror("table built");

	key_t k2 = ftok("queue", 2);
	int duty = semget(k2, 1, IPC_CREAT|0664);//для конца смены
	perror("duty started");

	//printf("%d", semid);

	struct sembuf put_dish = {0, -1, 0}, fill_sem = {0, TABLE_LIMIT, 0};
	struct sembuf start_duty = {0, 1, 0}, end_duty = {0, -1, 0};
	semop(semid, &fill_sem, 1);

	FILE *queue, *gost, *table; 
	char name[2048], dish_type[2048]; 
	queue = fopen ("queue", "r");
	semop(duty, &start_duty, 1);

	gost = fopen ("gost_washing", "r");
	table = fopen("table", "w");
 
	while (fscanf(queue, "%s %d", dish_type, &number)!=EOF){
		printf("Got: %s\n", dish_type);
		int delay = search(gost, dish_type);
		printf("Gost is: %d\n", delay);
		if (delay < 0) continue;

		for (int i = 0; i < number; i++){
			sleep(delay);
			semop(semid, &put_dish, 1);
			perror("semop");

			printf("washed: %s\n", dish_type);
			washed_dishes++;
			fprintf(table, "%s\n", dish_type);
			fflush(table);
		}
	}
	printf("Washed %d tableware.\n", washed_dishes);
	semop(duty, &end_duty, 1);
	return 0;
}
