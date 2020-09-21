#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

struct req_t{
        long mtype;
        int a, b;
	char op;
	pid_t src;
};

struct ans_t{
	long mtype;
	int c;
};

struct msgbuf{
	long mtype;
	char mtext[280];
};

int main(){
	key_t k = ftok("txt", 7);
	int p = msgget(k, IPC_CREAT|0664);
	perror("get");

	struct peq_t req1;
	req1.mtype = 1;
	req1.a = 2;
	req1.b = 4;
	req1.op = '/';

	int length = sizeof(req1) - sizeof(long);
	msgsnd(k, &req1, length, 0);

	struct msgbuf m0;
	int lbuf = sizeof(m0) - sizeof(long);

	int l = msgrcv(p, &m0, lbuf, 0, 0);
	perror("recieve");

	if( m0.mtype == 1 ){
		struct req_t* ptr = (struct req_t*)&m0;
		switch (ptr->op){
			case('+'):
			{
			}
			case('-'):
			{}
	}
return 0;
}
