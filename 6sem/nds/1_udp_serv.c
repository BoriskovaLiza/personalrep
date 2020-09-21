
/* A console-like (able to execute date, uname and ls) UDP server */

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main() {

	int  sockfd, clilen, n, fd[2];
	pid_t pid;
	char line[1000], output[1000];
	struct sockaddr_in servaddr, cliaddr;
    
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_port        = htons(51005);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	signal(SIGCHLD, SIG_IGN);
    
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		printf("Can\'t create socket, errno = %d\n", errno);
		exit(1);
	}

	if(bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
		printf("Can\'t bind socket, errno = %d\n", errno);
		close(sockfd);
		exit(1);
	}

	while(1) {
       
		clilen = sizeof(cliaddr);
       
		if( ( n = recvfrom(sockfd, line, 999, 0, (struct sockaddr *) &cliaddr, 
       &clilen)) < 0){
			printf("Can\'t receive request, errno = %d\n", errno);
			close(sockfd);
			exit(1);
		}

		line[n] = '\n'; 
		bzero(output, 1000);

		printf("from %s %s",inet_ntoa(cliaddr.sin_addr),line);

		if (strncmp(line, "date", 4)==0){
			
//			printf("got a command \'date\'");

			int stdin_copy = dup(0);
			int stdout_copy = dup(1);
			close(0); close(1);
			pipe(fd);

			pid = fork();

			if (pid == 0){
				execlp("date", "date", NULL);
			}
			else if (pid > 0){
				read(0, output, 1000);
				write(1, output, strlen(output));
			}
			else if (pid < 0){
				printf("Could not create a child/n");
			}
			close(*fd);
			dup2(stdin_copy, 0);
			dup2(stdout_copy, 1);
			close(stdin_copy); close(stdout_copy);
		}
		else if (strncmp(line, "ls", 2)==0){

			int stdin_copy = dup(0);
			int stdout_copy = dup(1);
			close(0); close(1);
			pipe(fd);

			pid = fork();

			if (pid == 0){
				execlp("ls", "ls", "-a", NULL);
			}
			else if (pid > 0){
				read(0, output, 1000);
				write(1, output, strlen(output));
			}
			else if (pid < 0){
				printf("Could not create a child/n");
			}
			close(*fd);
			dup2(stdin_copy, 0);
			dup2(stdout_copy, 1);
			close(stdin_copy); close(stdout_copy);
		}
		else if (strncmp(line, "uname", 5)==0){

			int stdin_copy = dup(0);
			int stdout_copy = dup(1);
			close(0); close(1);
			pipe(fd);

			pid = fork();

			if (pid == 0){
				execlp("uname", "uname", "-a", NULL);
			}
			else if (pid > 0){
				read(0, output, 1000);
				write(1, output, strlen(output));
			}
			else if (pid < 0){
				printf("Could not create a child/n");
			}
			close(*fd);
			dup2(stdin_copy, 0);
			dup2(stdout_copy, 1);
			close(stdin_copy); close(stdout_copy);
		}
		else {strcpy(output, "unknown command\n");}

		pid = fork();
		if (pid == 0){

			if(sendto(sockfd, output, strlen(output), 0, (struct sockaddr *) &cliaddr, 
       clilen) < 0){
				printf("Can\'t send answer, errno = %d\n", errno);
				close(sockfd);
				exit(1);
			}
		return 0;
		}

		if (pid < 0){
			printf("Could not create a child/n");
		}
	}
	return 0;
}
