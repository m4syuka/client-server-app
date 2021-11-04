#include "server.h"

const int MAX_SIZE_BUFF = 32768;

int main(int argc, char **argv)
{
	//парсинг аргументов 
	if(argc < 2)
	{
		printf("usage: ./*ur app* *keys* *file or dir*\n	for more information use -pomogite\n");
		return 0;
	}
	else if(!strcmp(argv[1],"-pomogite"))
	{
		printf("keys:\n");
		printf("	-dd share dir\n");
		printf("	-df share file\n");
		printf("	-u  download on PC\n");
		return 0;
	}
	else if (!strcmp(argv[1],"-dd"))
		selectDorF = 0;
	else if(!strcmp(argv[1],"-df"))
		selectDorF = 1;
	else if(!strcmp(argv[1],"-u"))
		selectDorF = 2;
	
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

	char serv_response [1024];
	//Если мы выбрали загрузку на сервер
	if (selectDorF == 2)
	{
		while(1)
		{
		addr_size = sizeof(their_addr);
		sock_client = accept(sock_serv,(struct sockaddr *)&their_addr,&addr_size);
		
		unsigned char postDataBuff[MAX_SIZE_BUFF];
		int postDataPacketSize = 0;
		//получаем запрос
		postDataPacketSize= recv(sock_client,postDataBuff,MAX_SIZE_BUFF,0);
		//если это GET, отправляем страничку с POST методом
		if(ParseHtml(postDataBuff,serv_response,0,0))
			SendData(sock_client,serv_response);
		//если это POST
		else if (strstr(postDataBuff,"POST"))
		{
			//***Начинаем пасрить заголовок POST запрос***
			
			//найдем размер файла (content-length) только зачем...?
			unsigned char *pos1, *pos2;
			pos1 = strstr(postDataBuff,"Content-Length: ") + 16;	
			long int postFileSize = strtol(pos1,NULL,10);
		
			//название файла
			pos1 = strstr(postDataBuff,"filename=\"") + 10;
			pos2 = strstr(postDataBuff,"\"\r\nContent-Type");
			char postFileName [512];
			strncpy(postFileName,pos1,pos2-pos1);
			
			//удаляем POST заголовок и записываем в файл
			fp = fopen(postFileName,"wb");
			pos1 = strstr(pos2,"\r\n\r\n") + 4;
			fwrite(pos1,1,postDataPacketSize - (pos1-postDataBuff),fp);
			
			//если размер пакета == размеру буфера, вероятнее всего, будут еще пакеты
			if(postDataPacketSize == MAX_SIZE_BUFF)
			{
				//последний пакет, вероятнее всего, будет < MAX_SIZE_BUFF
				while((postDataPacketSize= recv(sock_client,postDataBuff,MAX_SIZE_BUFF,0)) > 0)
				{
					//если так и есть
					if (postDataPacketSize < MAX_SIZE_BUFF)
					{
						//пишем в файл последний пакет - 64 символа(это blob)
						fwrite(postDataBuff,1,postDataPacketSize-64,fp);
						break;
					}
					fwrite(postDataBuff,1,postDataPacketSize,fp);
				}
			}
			fclose(fp);
				
		}	
		close(sock_client);
		}
	}
	else
	{
		//если расшариваем папку, то генерируем html страничку с файлами в папке
		if (selectDorF == 0)
			GenerationHtml(argv[2]);
		
		char getDataBuff[MAX_SIZE_BUFF];
		while(1)
		{	
			addr_size = sizeof(their_addr);
			sock_client = accept(sock_serv,(struct sockaddr *)&their_addr,&addr_size);
			
			//получаем первый пакет 
			recv(sock_client,getDataBuff,MAX_SIZE_BUFF,0);
			
			if(selectDorF == 0)
			{
				if(ParseHtml(getDataBuff,serv_response,argv[2],0))
					SendData(sock_client,serv_response);
			}
			else if (selectDorF == 1)
			{
				if(ParseHtml(getDataBuff,serv_response,0,argv[2]))
				SendData(sock_client,serv_response);
			}
		}
		close(sock_client);
	}
	close(sock_serv);
	return 0;
}

void SendData(int sock_client,const char *serv_response)
{
	//отправляем клиенту заголовок
	send(sock_client,serv_response,strlen(serv_response),0);

	//буффер для отправки файла
	unsigned char fileBuff[MAX_SIZE_BUFF];

	//отправляем клиенту сам файл
	for (int i = fileSize;i>=0; i-=MAX_SIZE_BUFF)
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
