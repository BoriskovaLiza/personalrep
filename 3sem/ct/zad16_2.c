#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

struct mymsg_t{
        long mtype;
        char s[255];
        double x;
};

struct msgbuf{
	long mtype;
	char mtext[280];
};

int main(){
	key_t k = ftok("txt", 0);
	int p = msgget(k, IPC_CREAT|0664);
	perror("get2");

	struct msgbuf m0;
	int lbuf = sizeof(m0) - sizeof(long);

	int l = msgrcv(p, &m0, lbuf, 0, 0);
	if( m0.mtype == 1 ){
		struct mymsg_t* ptr = (struct mymsg_t*)&m0;
		char* h = ptr->s;
		printf("%s", h);
	}
	perror("recieve");
return 0;
}
