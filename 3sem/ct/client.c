#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

struct req_t{
        long mtype;
	pid_t src;
        int a, b;
        char op;
};

struct ans_t{
        long mtype;
        double c;
};

struct msgbuf{
        long mtype;
        char mtext[280];
};

int main(){
        key_t k = ftok("txt", 8);
        int mid = msgget(k, IPC_CREAT|0664);
        perror("get");

	int pid = getpid();
        struct req_t req1 = {1, pid, 4, 3, '/'};

        int length = sizeof(req1) - sizeof(long);
        msgsnd(mid, &req1, length, 0);
	perror("send client");

        struct msgbuf m0;
        int lbuf = sizeof(m0) - sizeof(long);

	int l = msgrcv(mid, &m0, lbuf, pid, 0);
        perror("recieve client");

//	printf("%d", m0.mtype);
	if( 1 ){
        	struct ans_t* ptr = (struct ans_t*)&m0;
		printf("%f\n", ptr->c);
	}
	return 0;
}
