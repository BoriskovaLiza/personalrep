#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

int main(){
	char data[10];
	char message[5] = "pong";
	
	int sock_id = socket(AF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in me;
	bzero(&me, sizeof(me));
	me.sin_family = AF_INET;
	me.sin_port = htons(7777);
	me.sin_addr.s_addr = htonl(INADDR_ANY);
	//inet_aton("127.0.0.1", &dst.sin_addr);

	struct sockaddr_in src;
	int src_len = sizeof(src);

	int p = bind(sock_id, &me, sizeof(me));
	while( recvfrom(sock_id, data, 10, 0, &src, &src_len) ){
		p = sendto(sock_id, message, 5, 0, &src, src_len);
	}

	return 0;
}
