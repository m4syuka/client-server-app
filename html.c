#include "server.h"
#include <dirent.h>

void GenerationHtml(const char* dirInput)
{
	//открытие папки 
	dir = opendir(".");
	//создаем файл HtmlStartPage
	FILE* fpHtml = fopen("HtmlStartPage.html","w+");
	
	fprintf(fpHtml,"<html>\n");
	//название файлов в HtmlStartPage
	while(dir)
    {
        de = readdir(dir);
        if (!de) 
			break;
		//скрытые папки нам не нужны
		if(de->d_name[0] != '.')
		{
			fprintf(fpHtml,"<a href =\"%s\">%s</a>\n",de->d_name,de->d_name);
		}
		totalFileInDir ++;
    }
	printf("%d",totalFileInDir);
	fprintf(fpHtml,"</html>");
	//все закрываем
	fclose(fpHtml);
	closedir(dir);
}

int ParseHtml(char *buffClientRequest, char* serv_response,const char *dirInput)
{
	//Если это GET запрос
	if(strstr(buffClientRequest,"GET"))
	{
		//Если это запрос на конкретный файл
		dir = opendir(".");
		while(dir)
		{
			char *chch = malloc(1000);
			strcpy(chch,".//");
			de = readdir(dir);
			if (!de) 
				break;
			//скрытые папки нам не нужны
			if(de->d_name[0] != '.' && de->d_type == 8)
			{
				strcat(chch,de->d_name);
				if(strstr(buffClientRequest,"GET / HTTP/1.1"))
				{
					fp = fopen("HtmlStartPage.html","rb");
					fseek(fp,0L,SEEK_END);
					fileSize = ftell(fp);
					fseek(fp,0L,SEEK_SET);
					int checkFileWrite = 0;
					char chrFileSize [21];
					sprintf(chrFileSize,"%ld",fileSize);
					strcpy(serv_response,"HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Type: text/html\r\nContent-Length: ");
					strcat(serv_response,chrFileSize);
					strcat(serv_response,"\r\n\r\n\0");
					break;
				}
				if(strstr(buffClientRequest,de->d_name))
				{
					fp = fopen(chch,"rb");
					fseek(fp,0L,SEEK_END);
					fileSize = ftell(fp);
					fseek(fp,0L,SEEK_SET);
					int checkFileWrite = 0;
					char chrFileSize [21];
					sprintf(chrFileSize,"%ld",fileSize);
					strcpy(serv_response,"HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Type: text/plain\r\nContent-Length: ");
					strcat(serv_response,chrFileSize);
					strcat(serv_response,"\r\n\r\n\0");
				}
			}
		}
		return 1;
	}
	else
		return 0;
}
