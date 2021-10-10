#include "server.h"
#include <dirent.h>

void GenerationHtml(const char* dirInput)
{
	//открытие папки 
	dir = opendir(dirInput);
	//создаем файл HtmlStartPage
	FILE* fpHtml = fopen("HtmlStartPage.html","w+");
	
	fprintf(fpHtml,"<html><meta charset=\"utf-8\"><ol>\n");
	//название файлов в HtmlStartPage
	while(dir)
    {
        de = readdir(dir);
        if (!de) 
			break;
		//скрытые папки нам не нужны
		if(de->d_name[0] != '.' && de->d_type == 8)
		{
			char chrFileSize [21];
			char *chch = (char*) malloc(1000);
			strcpy(chch,dirInput);
			strcat(chch,"//");
			strcat(chch,de->d_name);
			fp = fopen(chch,"rb");
			GetFileSize(fp,chrFileSize);
			free(chch);
			fprintf(fpHtml,"<li><a href =\"%s\">%s</a> SizeFile = %s bytes</li>\n",de->d_name,de->d_name,chrFileSize);
		}
		totalFileInDir ++;
    }
	fprintf(fpHtml,"</ol></html>");
	//все закрываем
	fclose(fpHtml);
	closedir(dir);
}

int ParseHtml(char *buffClientRequest, char* serv_response,const char *dirInput,const char* fileInput)
{
	//Если это GET запрос
	if(strstr(buffClientRequest,"GET"))
	{
		//Если это запрос на конкретный файл
		if (selectDorF == 0)
		{
			dir = opendir(dirInput);
			while(dir)
			{
				de = readdir(dir);
				if (!de) 
					break;
				//скрытые файлы и папки нам не нужны
				if(de->d_name[0] != '.' && de->d_type == 8)
				{
					//если это обычный ГЕТ запрос, то отправляем стартовую страницу
					if(strstr(buffClientRequest,"GET / HTTP/1.1"))
					{
						fp = fopen("HtmlStartPage.html","rb");
						char chrFileSize [21];
						GetFileSize(fp,chrFileSize);
						strcpy(serv_response,"HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Type: text/html\r\nContent-Length: ");
						strcat(serv_response,chrFileSize);
						strcat(serv_response,"\r\n\r\n\0");
						break;
					}
					//если в запросе есть есть имя файла из папки, то отправялем его 
					else if(strstr(buffClientRequest,de->d_name))
					{
						char *chch = (char*) malloc(1000);
						strcpy(chch,dirInput);
						strcat(chch,"//");
						strcat(chch,de->d_name);
						fp = fopen(chch,"rb");
						free(chch);
						char chrFileSize [21];
						GetFileSize(fp,chrFileSize);
						strcpy(serv_response,"HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Type: text/plain\r\nContent-Disposition: attachment; filename=\"");
						strcat(serv_response,de->d_name);
						strcat(serv_response,"\"\r\nContent-Length: ");
						strcat(serv_response,chrFileSize);
						strcat(serv_response,"\r\n\r\n\0");
					}
					//если это ГЕТ запрос и выбрали один файл 
				}
			}
		}
		else if(selectDorF == 1)
		{
			if (strstr(buffClientRequest,"GET / HTTP/1.1"))
			{
				fp = fopen(fileInput,"rb");
				char chrFileSize [21];
				GetFileSize(fp,chrFileSize);
				strcpy(serv_response,"HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\nContent-Type: text/plain\r\nConnection: Keep-Alive\r\nContent-Disposition: attachment; filename=\"");
				strcat(serv_response,fileInput);
				strcat(serv_response,"\"\r\nContent-Length: ");
				strcat(serv_response,chrFileSize);
				strcat(serv_response,"\r\n\r\n\0");
			}
		}
		return 1;
	}
	else
		return 0;
}

void GetFileSize(FILE *fp, char *chrFileSize)
{
	fseek(fp,0L,SEEK_END);
	fileSize = ftell(fp);
	fseek(fp,0L,SEEK_SET);
	sprintf(chrFileSize,"%ld",fileSize);
}
