
/* A simple echo UDP client */

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{

    int  sockfd, n, len;
    char sendline[1000], recvline[1000];
    struct sockaddr_in servaddr, cliaddr;
    unsigned short port;
    
    if(argc < 2 || argc > 3){
       printf("Usage: a.out <IP address> <port - default 51000>\n");
       exit(1);
    }
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    if(argc == 3){
       port = atoi(argv[2]);
       if(port == 0){
           printf("Invalid port\n");
           exit(-1);
       }
    } else {
       port = 51000;
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
    
    printf("String => ");
    fgets(sendline, 1000, stdin);
    
    if(sendto(sockfd, sendline, strlen(sendline)+1, 0, (struct sockaddr *) &servaddr, 
    sizeof(servaddr)) < 0){
       printf("Can\'t send request, errno = %d\n", errno);
       close(sockfd);
       exit(1);
    }
    
    if((n = recvfrom(sockfd, recvline, 1000, 0, (struct sockaddr *) NULL, NULL)) < 0){
       printf("Can\'t receive answer, errno = %d\n", errno);
       close(sockfd);
       exit(1);
    }

    recvline[n] = 0;
    
    printf("%s", recvline);
    close(sockfd);
    return 0;
}
