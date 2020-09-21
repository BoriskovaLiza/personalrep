#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#include <errno.h>

#include <fcntl.h>

#include<netinet/in.h>
#include<arpa/inet.h>

void hndl_chld(int signum){
	int status = 0;
	waitpid(-1, &status, WNOHANG);
	if(WIFEXITED(status)){
		printf("Exited with %d\n", WEXITSTATUS(status));
	}
	else if(WIFSIGNALED(status)){
		printf("Terminated with %d\n", WTERMSIG(status));
	}
}

int main(){

	signal(SIGCHLD, hndl_chld);

	char data[10];
	char message[5] = "kek";
	int status;
	
	int sock_id = socket(PF_INET, SOCK_STREAM, 0);
	int sock_new;

	struct sockaddr_in me;
	bzero(&me, sizeof(me));
	me.sin_family = AF_INET;
	me.sin_port = htons(7777);
	me.sin_addr.s_addr = htonl(INADDR_ANY);

	int p = bind(sock_id, (struct sockaddr*)&me, sizeof(me));
	perror("bind");

	p = listen(sock_id, 20);
	perror("listen");

	struct sockaddr_in src;
	int src_len = sizeof(src);

	while( sock_new = accept(sock_id, &src, &src_len) ){
		if (fork()==0){
			printf("client\n");
			read(sock_new, data, 10);
			write(sock_new, message, 5);
			close(sock_new);
			return 0;
		}
	}

	return 0;
}
