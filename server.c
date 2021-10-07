
#include "server.h"

const int MAX_SIZE_BUFF = 8192;

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("2 arg\n");
		return -1;
	}
	//установка соединения
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
	
	char buffClientRequest[1024];
	int result_response;

	//Генерируем HtmlStartPage 
	GenerationHtml();
	

	
	//формируем первый ответ
	char serv_response [1000];
	
	unsigned char fileBuff[MAX_SIZE_BUFF];
	
		
	while(1)
	{	
		addr_size = sizeof(their_addr);
		sock_client = accept(sock_serv,(struct sockaddr *)&their_addr,&addr_size);
		
		result_response = recv(sock_client,buffClientRequest,1024,0);
		if(result_response == SO_ERROR)
			printf("recv fail");
		else if (result_response > 0)
		{
			buffClientRequest[result_response] = '\0';
			if(ParseHtml(buffClientRequest,serv_response,argv[1]) == 1)
			{
				send(sock_client,serv_response,strlen(serv_response),0);

				for (int i = fileSize;i>=0; i-=MAX_SIZE_BUFF )
				{
					if(fileSize<MAX_SIZE_BUFF)
					{
						fread(fileBuff,1,i,fp);
						send(sock_client,fileBuff,i,0);
					}
					else
					{
						fread(fileBuff,1,MAX_SIZE_BUFF,fp);
						send(sock_client,fileBuff,MAX_SIZE_BUFF,0);
					}
				}
			}
		}
		close(sock_client);
	}
	close(sock_serv);

	return 0;
}