#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

//другое 
extern const int MAX_SIZE_BUFF;

//для открытия папки
DIR *dir;
struct dirent *de;
int totalFileInDir;

//Открытие файла для чтения
FILE *fp;
long int fileSize;

void GenerationHtml();
void SendStartHtml(char *serv_response);
int ParseHtml(char *buffClientRequest,char *serv_response,const char *dirInput);
