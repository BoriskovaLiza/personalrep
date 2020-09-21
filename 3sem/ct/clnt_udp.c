#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <time.h>

#include<netinet/in.h>
#include<arpa/inet.h>

int main(){
	char data[10];
	char message[5] = "ping";
	
	int sock_id = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in me;
	bzero(&me, sizeof(me));
	me.sin_family = AF_INET;
	me.sin_port = htons(0);
	me.sin_addr.s_addr = htonl(ADDR_ANY);
	//inet_aton("127.0.0.1", &me.sin_addr);

	struct sockaddr_in to;
	bzero(&to, sizeof(to));
	to.sin_family = AF_INET;
	to.sin_port = htons(7777);
	inet_aton("127.0.0.1", &to.sin_addr);

	int p = bind(sock_id, &me, sizeof(me));

	p = sendto(sock_id, message, 5, 0, &to, sizeof(to));
	recvfrom(sock_id, data, 10, 0, NULL, NULL);

	printf("%s", data);
	//sleep(1);

	return 0;
}
