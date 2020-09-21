#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum){
	printf("Ha-Ha\n");
}

int main(){
	signal(SIGINT, handler);
	while(1){
		sleep(1);
	}
}
