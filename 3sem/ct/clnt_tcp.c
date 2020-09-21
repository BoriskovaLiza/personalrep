#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include <fcntl.h>

#include<netinet/in.h>
#include<arpa/inet.h>

int main(){
	char data[10];
	char message[5] = "ping";
	
	int sock_id = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in me;
	bzero(&me, sizeof(me));
	me.sin_family = AF_INET;
	me.sin_port = htons(0);
	me.sin_addr.s_addr = htonl(INADDR_ANY);

	struct sockaddr_in to;
	bzero(&to, sizeof(to));
	to.sin_family = AF_INET;
	to.sin_port = htons(7777);
	inet_aton("10.55.83.60", &to.sin_addr);

	connect(sock_id, &to, sizeof(me));

	write(sock_id, message, 5);
	read(sock_id, data, 10);

	printf("%s", data);
	sleep(1);

	return 0;
}
