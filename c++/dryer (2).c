#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>

#include <fcntl.h>
#include <stdio.h>

#include <errno.h>
#include <wait.h>

int search(FILE *file, char* dish_type){
		int delay = 0;
		char name[2048];
		rewind(file);
		while(fscanf(file, "%s %d", name, &delay)!=EOF){
			if (strcmp(name, dish_type) == 0) return delay;
		}
		printf("Dryer: unknown type of tableware: %s", dish_type);
		sleep(3);
		return -1;//не нашли посудку
};

int main(){

        int dryed_dishes = 0;
	
	key_t k = ftok("queue", 1);
	int semid = semget(k, 1, IPC_CREAT|0664);//для количества посуды на столе
	perror("table built");

	key_t k2 = ftok("queue", 2);
	int duty = semget(k2, 1, IPC_CREAT|0664);//для конца смены
	perror("duty started");

	struct sembuf take_dish = {0, 1, 0}, is_duty_over = {0, 0, IPC_NOWAIT};

	FILE *gost, *table; 
	char name[2048], dish_type[2048];
	gost = fopen ("gost_drying", "r");
	table = fopen("table", "r");

	while (	semop(duty, &is_duty_over, 1) < 0){
		while(fscanf(table, "%s", dish_type)!=EOF){
			printf("Got: %s\n", dish_type);
			int delay = search(gost, dish_type);
			printf("Gost is: %d\n", delay);
	
			if (delay < 0) continue;
			semop(semid, &take_dish, 1);
			sleep(delay);
			printf("dryed: %s\n", dish_type);
			dryed_dishes++;
		}
	}

	printf("Dryed %d tableware.\n", dryed_dishes);
	return 0;
	//удалить все из table(затереть)
}
