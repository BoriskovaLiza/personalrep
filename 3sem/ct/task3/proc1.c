#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <wait.h>

int alive_chld = 0;

void handle_chld(int signum){
	int status = 0;
	while( waitpid(-1, &status, WNOHANG) ) > 0 )
		alive_chld--;
	
	if(alive_chld < 0){
		printf("The number of running procecces is below zero.\n");
	}
}

int main( int argc, char* argv[] ){

	if (argc != 2)
	{
		printf("Wrong input\n");
		return -1;
	}

	int max_chld = atoi (argv[1]);

	char* argv[30];
	char line[2048];
	char sep[10] = " ";
	char* istr;
	pid_t p;
	int status = 0, j = 0;

	signal(SIGCHLD, handle_chld);

	while( gets(line) ){
			printf("Got: %s\n", line);
			istr = strtok(line, sep);
			int i = 0;
			while(istr!=NULL){
				argv[i] = istr;
				i++;
				istr = strtok(NULL, sep);
			}
			argv[i] = NULL;
			if (max_chld <= alive_chld){
				printf("You've already run %d processes.\n", alive_chld);
				continue;
			}
			p = fork();
			if (p == 0){
				execvp(argv[0], argv);
				printf("Unknown command.\n");
				exit(-1);
			}
			else{
				alive_chld++;
			}
	}

	return 0;
}
