#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>

struct mymsg_t{
        long mtype;
        char s[255];
        double x;
};


int main(){

	key_t k = ftok("txt", 0);
	int p = msgget(k, IPC_CREAT|0664);
	perror("get");

	struct mymsg_t m0;
	m0.mtype = 1;
	strcpy(m0.s, "blah-blah-blah");
	m0.x = 3.1415;

	int length = sizeof(m0) - sizeof(long);

	int l = msgsnd(p, &m0, length, 0);
	perror("send");
return 0;
}
