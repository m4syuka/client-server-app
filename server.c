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
	if((s = getaddrinfo(NULL,"http",&hints,&serverinfo)) != 0)
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
		if(setsockopt(sock_serv,SOL_SOCKET,SO_REUSEADDR,&int_perem,sizeof(int)) == -1)
			perror("setsockopt");
		if(bind(sock_serv,res->ai_addr,res->ai_addrlen) == -1)
			perror("bind");

	}

	if(listen(sock_serv,10) == -1)
		perror("listen");

	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	int sock_client;
	
	char buff[1024];
	int result_response;
	//формирвоание выдачи
	char serv_response [65536] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 100\r\n\r\n\<html>\n<head>\n<title>321</title>\n</head>\n<body>123<a href = \"server.c\">server</a></body>\n</html>";//= malloc(63 + length_num + 100);
	
	while(1)
	{
		addr_size = sizeof(their_addr);
		sock_client = accept(sock_serv,(struct sockaddr *)&their_addr,&addr_size);
		
		result_response = recv(sock_client,buff,1024,0);
		if(result_response == SO_ERROR)
			printf("recv fail");
		else if (result_response > 0)
		{
			buff[result_response] = '\0';
			send(sock_client,serv_response,65536,0);
		}
		close(sock_client);
	}
	close(sock_serv);

	return 0;
}
