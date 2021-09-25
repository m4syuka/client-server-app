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
#include <errno.h>

int main()
{
	struct addrinfo hints = {0}, *serverinfo,*res;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int s;
	if((s = getaddrinfo(NULL,"1233",&hints,&serverinfo)) != 0)
	{
		printf("%s",gai_strerror(s));
		return 1;
	}

	int sock_serv;
	int int_perem = 1;
	for (res = serverinfo;res != 0; res = res->ai_next)
	{
		if((sock_serv = socket(res->ai_family,res->ai_socktype,res->ai_protocol)) == -1)
			perror("socket");
		if(bind(sock_serv,res->ai_addr,res->ai_addrlen) == -1)
			perror("bind");
		if(setsockopt(sock_serv,SOL_SOCKET,SO_REUSEADDR,&int_perem,sizeof(int)) == -1)
			perror("setsockopt");
	}

	if(listen(sock_serv,10) == -1)
		perror("listen");

	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	int sock_client;
	
	while(1)
	{
		addr_size = sizeof(their_addr);
		sock_client = accept(sock_serv,(struct sockaddr *)&their_addr,&addr_size);
		send(sock_client,"privet",6,0);
		close(sock_client);
	}



	return 0;
}
