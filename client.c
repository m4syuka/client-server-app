#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc,char *argv[])
{

	struct addrinfo hints = {0}, *res;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	int status;
	if((status = getaddrinfo("192.168.1.170","1233", &hints, &res)) != 0)
	{
		printf("getaddrinfo: %s\n",gai_strerror(status));
	}

	int c_socket;
	if((c_socket = socket(res->ai_family,res->ai_socktype,res->ai_protocol)) == -1)
		perror("socket");
	if((connect(c_socket,res->ai_addr,res->ai_addrlen)) == -1)
		perror("connect");

	char buf[100];
	recv(c_socket,buf,100,NULL);
	buf[100] = '\0';
	printf("%s\n",buf);
	
	return 0;

}
