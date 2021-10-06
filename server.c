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
	
	//узнаем размер файла
	FILE *fp = fopen("1.jpg","rb");
	fseek(fp,0L,SEEK_END);
	int fileSize = ftell(fp);
	fseek(fp,0L,SEEK_SET);
	unsigned char fileBuff[4096];
	int checkFileWrite = 0;
	
	char chrFileSize [21];
	sprintf(chrFileSize,"%d",fileSize);
	
	//формируем ответ
	char serv_response [1000] = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Type: image/jpg\r\nContent-Length: ";//\r\n<html>\n<head>\n<title>321</title>\n</head>\n<body>123<a href = \"server.c\">server</a></body>\n</html>";//= malloc(63 + length_num + 100);
	strcat(serv_response,chrFileSize);
	strcat(serv_response,"\r\n\r\n");
	
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
			printf("%s\n",buff);
			if(strstr(buff,"GET"))
			{
				//2745
				send(sock_client,serv_response,strlen(serv_response),0);

				for (int i = fileSize;i>=0; i-=4096 )
				{
					if(fileSize<4096)
					{
						fread(fileBuff,1,i,fp);
						send(sock_client,fileBuff,i,0);
					}
					else
					{
						fread(fileBuff,1,4096,fp);
						send(sock_client,fileBuff,4096,0);
					}
				}
				fseek(fp,0L,SEEK_SET);
			}
		}
		close(sock_client);
	}
	close(sock_serv);

	return 0;
}