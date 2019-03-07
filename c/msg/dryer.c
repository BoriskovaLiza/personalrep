#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <unistd.h>
#include <string.h>
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


struct dish{
        long mtype;
        char dish_type[256];
};

struct msgbuf{
        long mtype;
        char s[256];
};

int main(){

	key_t k = ftok("queue", 3);
	int mid = msgget(k, IPC_CREAT|0664);
	perror("dryer_get");

	FILE *input, *gost; 
	char name[2048]; 
	input = fopen ("queue", "r");
	gost = fopen ("gost_drying", "r");

        struct dish ans;
	ans.mtype = 3;

	struct msgbuf m0;
        int lbuf = sizeof(m0) - sizeof(long);

	while(1){
		int l = msgrcv(mid, &m0, lbuf, 4, 0);
	        struct dish* ptr = (struct dish*)&m0;
		if (strcmp(ptr->dish_type, "Duty ended.") == 0)
			break;
		printf("Got: %s\n", ptr->dish_type);
		msgsnd(mid, &ans, lbuf, 0);
		int delay = search(gost, ptr->dish_type);
		sleep(delay);
		printf("Dryed: %s\n", ptr->dish_type);
	}
	return 0;
}
