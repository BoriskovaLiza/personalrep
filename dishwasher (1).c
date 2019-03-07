#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <unistd.h>
#include <stdlib.h>

#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#include <wait.h>
#include <errno.h>

//const int TABLE_LIMIT = 5;

int search(FILE *file, char* dish_type){
		int delay = 0;
		char name[2048];
		rewind(file);
		while(fscanf(file, "%s %d", name, &delay)!=EOF){
			if (strcmp(name, dish_type) == 0) return delay;
		}
		printf("Dishwasher: unknown type of tableware: %s\n", dish_type);
		sleep(3);
		return -1;//ненашли посудку
};

struct dish{
        long mtype;
        char dish_type[256];
};

struct msgbuf{
        long mtype;
        char s[256];
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

	int number = 0;
	int table_filled = 0;
	int status;
	pid_t pid;
	
	key_t k = ftok("queue", 3);
	int mid = msgget(k, IPC_CREAT|0664);
	perror("washer_get");

	struct msgbuf m0;
	int lbuf = sizeof(m0) - sizeof(long);

	struct dish req;
	req.mtype = 4;

	FILE *input, *gost; 
	char name, dish_t[2048]; 
	input = fopen ("queue", "r");
	gost = fopen ("gost_washing", "r");
 
	while (fscanf(input, "%s %d", dish_t, &number) == 2){
		//обязательно написать EOF как eof в файле!
		int delay = search(gost, dish_t);
		printf("Got: %s %d\n", dish_t, number);
		for (int i = 0; i < number; i++){
			sleep(delay);
			if (table_filled >= TABLE_LIMIT){
				msgrcv(mid, &m0, lbuf, 3, 0);
				table_filled--;
			}
			strcpy(req.dish_type, dish_t);
			msgsnd(mid, &req, lbuf, 0);
			table_filled++;
			printf("Washed: %s\n", dish_t);
		}
	}

	strcpy(req.dish_type, "Duty ended.");
	msgsnd(mid, &req, lbuf, 0);

	for (int i; i < TABLE_LIMIT; i++){
		msgrcv(mid, &m0, lbuf, 3, 0);
	}

	return 0;
}
