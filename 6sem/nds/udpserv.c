
/* A simple echo UDP server */

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{

    int  sockfd, clilen, n;
    char line[1000];
    struct sockaddr_in servaddr, cliaddr;
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(51005);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
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
       printf("from %s %s",inet_ntoa(cliaddr.sin_addr),line);
       if(sendto(sockfd, line, strlen(line), 0, (struct sockaddr *) &cliaddr, 
       clilen) < 0){
          printf("Can\'t send answer, errno = %d\n", errno);
          close(sockfd);
          exit(1);
       }
    }
    return 0;
}
