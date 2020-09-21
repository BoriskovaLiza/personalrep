#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

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

	struct msgbuf m0;
	int lbuf = sizeof(m0) - sizeof(long);

	while (1){
		int l = msgrcv(mid, &m0, lbuf, 1, 0);
		perror("recieve server");

		if( m0.mtype == 1){
			struct req_t* ptr = (struct req_t*)&m0;
			struct ans_t ans;
			ans.mtype = ptr->src;
//			printf("%d", ans.mtype);
			switch (ptr->op){
				case('+'):
					ans.c = ptr->a + ptr->b;
					break;
				case('-'):
                                        ans.c = ptr->a - ptr->b;
                                        break;
				case('*'):
                                        ans.c = ptr->a * ptr->b;
                                        break;
				case('/'):
                                        ans.c = ptr->a / ptr->b;
                                        break;
				default:
					printf("Something went wrong\n%d", ptr->op);
					return 1;
			}

			int length = sizeof(ans) - sizeof(long);
			msgsnd(mid, &ans, length, 0);
			perror("send server");
		}

	}
return 0;
}
