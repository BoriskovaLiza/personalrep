
/* Pi parallel counting UDP server (compile with option -lm)*/

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include <math.h>

int main() {

	int  sockfd, clilen, n;
	pid_t pid;
	int N, ibeg, iend, i;
	char line[1000];
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
		
		N = 1000;
		ibeg = 0;
		iend = 1000;

		N = atoi(strtok(line, " "));
		ibeg = atoi(strtok(NULL, " "));
		iend = atoi(strtok(NULL, " "));

		double h = 2.0 / N;
		double tmp = 0.;

		printf("Computing from %d to %d\n", ibeg, iend);

		for (i = ibeg; i <= iend - 1; i++){
			tmp += 0.5*(sqrt(4.0 - i*h*i*h) + sqrt(4.0-((i+1)*h)*((i+1)*h)))*h;
		}

		sprintf(line, "%f", tmp);

		pid = fork();
		if (pid == 0){

			if(sendto(sockfd, line, strlen(line), 0, (struct sockaddr *) &cliaddr, clilen) < 0){
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
