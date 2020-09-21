
/* Pi parallel counting UDP client */

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

int main(int argc, char **argv)
{

	int  sockfd, n, len, i;
	int N, N_proc;
	int ibeg, iend;
	double pi;
	char sendline[1000], recvline[1000];
	struct sockaddr_in servaddr, cliaddr;
	unsigned short port;
    
	if(argc < 2 || argc > 5){
		printf("Usage: a.out <IP address> <port - default 51005> <Number of sections - default 1000> <Number of processes - default 1>\n");
		exit(1);
	}
    
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;

	N = 1000;
	N_proc = 1;
	pi = 0;

	if(argc > 2){
		port = atoi(argv[2]);
		if(port == 0){
			printf("Invalid port\n");
			exit(-1);
		}
		if(argc > 3) {N = atoi(argv[3]); if(N <= 0) N = 1000;}
		if(argc > 4) {N_proc = atoi(argv[4]); if(N_proc <= 0) N_proc = 1;}
	} else {
		port = 51005;
	}
	
	
	servaddr.sin_port   = htons(port);
    
	if(inet_aton(argv[1], &servaddr.sin_addr) == 0){
		printf("Invalid IP address\n");
		exit(1);
	}

	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		printf("Can\'t create socket, errno = %d\n", errno);
		exit(1);
	}

	bzero(&cliaddr, sizeof(cliaddr));
	cliaddr.sin_family      = AF_INET;
	cliaddr.sin_port        = htons(0);
	cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
	if(bind(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) < 0){
		printf("Can\'t bind socket, errno = %d\n", errno);
		close(sockfd);
		exit(1);
	}

	bzero(sendline, 1000);
    
	for (i = 0;  i < N_proc; i++){
		ibeg = (int)N/N_proc*i;
		iend = (int)N/N_proc*(i+1);
		if (i == N_proc-1) {iend = N;}
		sprintf(sendline, "%d %d %d ", N, ibeg, iend);

		if(sendto(sockfd, sendline, strlen(sendline)+1, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
			printf("Can\'t send request, errno = %d\n", errno);
			close(sockfd);
			exit(1);
		}
	}
	
	for (i = 0;  i < N_proc; i++){
		if((n = recvfrom(sockfd, recvline, 1000, 0, (struct sockaddr *) NULL, NULL)) < 0){
			printf("Can\'t receive answer, errno = %d\n", errno);
			close(sockfd);
			exit(1);
		}
		pi += atof(recvline);
	}
    
	printf("Got %f\n", pi);
	close(sockfd);
	return 0;
}
