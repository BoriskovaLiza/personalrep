#include <sys/types.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

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

	if (fork() == 0){
		return -123;
	}

	if (fork() == 0){
	printf("Child pid %d\n", getpid());
        	while(1){
                	sleep(1);
        	}
	}

	while(1){
		sleep(1);
	}
}
