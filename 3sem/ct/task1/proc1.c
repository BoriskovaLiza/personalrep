#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
int main(){
	FILE *input;
	int delay; 
	char name[2048]; 
	input = fopen ("input.txt", "r"); 
	while (fscanf(input, "%d %s", &delay, name)!=EOF) {
		pid_t p=fork();
		if (p == 0){
			sleep(delay); 
			execlp(name, name, NULL); 
		}
	}
	return 0;
}
