#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <signal.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#define VERSION 23
#define BUFSIZE 946494
#define ERROR 42
#define LOG 44
#define FORBIDDEN 403
#define NOTFOUND 404
#define port 6152
#define DISPLAYHT "Display.html"
using namespace std;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int good_requests,bad_requests,downloads;
//Initially defining every module
void FileLog(int sd, char* buffer);
void sys_details();
extern void FilesUpload(int,char*,int);
void FilesRequest(int);
extern void FilesDisplay(int,char*);
extern void FilesShare(int,char*);
int main(int argc, char **argv)
{
//socket framework
 int status;
 pthread_t threads[100];
 int i,listenfd, sd;
 socklen_t len;
 struct sockaddr_in cli_addr;
 struct sockaddr_in serv_addr;
 pthread_mutex_init(&mutex,0);
 listenfd = socket(PF_INET, SOCK_STREAM,0);
 int opt =1;
 setsockopt(listenfd, SOL_SOCKET,  SO_REUSEADDR | SO_REUSEPORT, &opt, 4);
 if(port< 0 || port >65535)
    printf("==============Invalid port number (try 1->65535)==============");
    printf("Port address is: %d\n", port);
//socket description
 serv_addr.sin_family = PF_INET;
 serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 serv_addr.sin_port = htons(port);
//socket binding
 if(bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0)
   {
    printf("==============system call bind error==============\n");
    exit(0);
   }
//socket listening
 if( listen(listenfd,5) <0)
 {
  printf("==============system call listen error==============");
  exit(0);
 }
 printf("Server started...waiting for client...\n");
 fflush(stdout);
 for(i=1; ;i++)
 {
  len = sizeof(cli_addr);
//accept criteria
    if((sd = accept(listenfd, (struct sockaddr *)&cli_addr, &len)) < 0)
	{
	 printf("==============system call accept error==============");
	 exit(0);
	}
if (pthread_create(&threads[i], NULL, (void * (*)(void *))FilesRequest, (void *)sd) < 0)
	printf("==============Error creating thread==============");
	printf("Thread created:%ld\n",threads[i]);
//Thread numbers assigned
	sys_details();
	sleep(1);
 }

}
void sys_details()
{
//cout<<"Hiiii\n";
  cout<<endl;
  cout<<"============Server Statistics==========="<<endl;
  printf("Number of successful Requests: %d\n",good_requests);
  fflush(stdout);
  printf("Number of bad Requests: %d\n",bad_requests);
  fflush(stdout);
}
// Files REQUEST
extern pthread_mutex_t mutex;
extern int good_requests,bad_requests;
extern int downloads;

using namespace std;
//Files Request module
void FilesRequest(int sd)
{
 int j,fd,blen,tlen;
 long i,rt,len;
 const char * type;
 static char buffer[BUFSIZE+1],temp_buffer[BUFSIZE+1]; //zero filled static
 struct stat s_buf;
 rt =read(sd,buffer,BUFSIZE); 	//read the request
 string extension = "cpp";
 bzero(temp_buffer,sizeof(temp_buffer));
 strcpy(temp_buffer,buffer);
 tlen = strlen(temp_buffer);
//Upload called
 if( !strncmp(&temp_buffer[6],"FilesUpload", 11)) {
	cout<<"==============Upload Called=============="<<endl;
	FilesUpload(sd,buffer,rt);
			pthread_exit(NULL);
				}

	if(rt > 0 && rt < BUFSIZE)	
	buffer[rt]=0;		//empty the buffer
	else buffer[0]=0;
	for(i=0;i<rt;i++)	
		if(buffer[i] == '\r' || buffer[i] == '\n')
			buffer[i]='*';

	if( strncmp(buffer,"GET ",4) && strncmp(buffer,"get ",4) ) {
	printf("Only simple GET operation supported:%s",buffer);
	}

	for(i=4;i<BUFSIZE;i++) { 
       //null ending after the second space
	if(buffer[i] == ' ') { 
       // It is like "GET URL "+other things
		buffer[i] = 0;
		break;
	}
	}

	//Extensions supported by the WebServer
	if( !strncmp(&buffer[0],"GET /\0",6) || !strncmp(&buffer[0],"get /\0",6) ) 
	{
	(void)strcpy(buffer,"GET /Login.html");
	pthread_mutex_lock(&mutex);
	good_requests += 1;
	pthread_mutex_unlock(&mutex);
	}
	blen=strlen(buffer);
	type = (char *)0;

	extension = "php";
	//printf("%s",buffer);
	len = strlen(extension.c_str());
	if( !strncmp(&buffer[blen-len], extension.c_str(), len)) {
			type = "text/php";
			//break;
	}


	extension = "html";
	//printf("%s",buffer);
	len = strlen(extension.c_str());
	if( !strncmp(&buffer[blen-len], extension.c_str(), len)) {
		type = "text/html";
		//break;
	}

	extension = "jpg";
	len = strlen(extension.c_str());
	if( !strncmp(&buffer[blen-len], extension.c_str(), len)) {
		type = "image/jpeg";
		//break;
	}


	extension = "gif";
	len = strlen(extension.c_str());
	if( !strncmp(&buffer[blen-len], extension.c_str(), len)) {
		type = "image/gif";
		//break;
	}


	extension = "css";
	len = strlen(extension.c_str());
	if( !strncmp(&buffer[blen-len], extension.c_str(), len)) {
		type = "text/css";
		//break;
	}

	extension = "woff";
	len = strlen(extension.c_str());
	if( !strncmp(&buffer[blen-len], extension.c_str(), len)) {
		type = "font/woff";
		//break;
	}
	extension = "pdf";
	len = strlen(extension.c_str());
	if( !strncmp(&buffer[blen-len], extension.c_str(), len)) {
		type = "application/pdf";
		//break;
	}
	extension = "txt";
	//printf("%s",buffer);
	len = strlen(extension.c_str());
	if( !strncmp(&buffer[blen-len], extension.c_str(), len)) {
		type = "text/plain";
		//break;
	}
	extension = "ico";
	//printf("%s",buffer);
	len = strlen(extension.c_str());
	if( !strncmp(&buffer[blen-len], extension.c_str(), len)) {
		type = "image/x-icon";
		//break;
	}
//Calling the required modules
	extension = "cpp";
	//char t_buf[strlen(buffer)];
	bzero(temp_buffer,sizeof(temp_buffer));
	strcpy(temp_buffer,buffer);
	for(i=4;i<BUFSIZE;i++) { 
//null ending after the second space
	if(temp_buffer[i] == '?') { 
//It is like 'GET URL '+other things
		temp_buffer[i] = 0;
		break;
	}
	}
	len = strlen(extension.c_str());
	tlen = strlen(temp_buffer);
	if( !strncmp(&temp_buffer[tlen-len], extension.c_str(), len)) {

//Service for login
		if( !strncmp(&temp_buffer[5],"FileLog", 7))
		{
			FileLog(sd,buffer);
			cout<<"==============Server called=============="<<endl;
 /*pthread_mutex_lock(&mutex);
    good_requests += 1;
    //printf("Successful=%d\n",good_requests);
    pthread_mutex_unlock(&mutex);
    return 0;*/
			pthread_exit(NULL);
		}
//Service for Dynamic Display File name html to share
		else if( !strncmp(&temp_buffer[5],"FilesDisplay", 12))
	   	{
			cout<<"==============Welcome to Centralized File Sharing System=============="<<endl;
			FilesDisplay(sd,buffer);
			cout<<""<<endl;
/* pthread_mutex_lock(&mutex);
    good_requests += 1;
    //printf("Successful=%d\n",good_requests);
    pthread_mutex_unlock(&mutex);
    return 0;*/
	   	}
//Service to share the file to other users
		else if( !strncmp(&temp_buffer[5],"FilesShare", 10))
	   	{
			cout<<"==============Share with=============="<<endl;
			FilesShare(sd,buffer);
			//cout<<"SHARE ENDDD"<<endl;
			pthread_exit(NULL);
	   	}

}

	if(type == 0){
	pthread_mutex_lock(&mutex);
	bad_requests += 1;
	pthread_mutex_unlock(&mutex);
	printf("==============file extension type supported:%s==============\n",buffer);
	len = 25;
		(void)sprintf(buffer,"HTTP/1.1 204 No Content\nServer: 207httpd/%d.0\nContent-len: %ld\nConnection: close\nContent-Type: %s\n\n", VERSION,len, type);
	pthread_exit(NULL);
	}

	if(( fd = open(&buffer[5],O_RDONLY)) == -1)
	{
	printf("==============opening the requested file==============\n");
	fflush(stdout);
	pthread_exit(NULL);
	}
	else
	{
	int rec = fstat(fd, &s_buf);
	if (rec < 0)printf("==============Error in opening the requested file==============\n");
	fflush(stdout);
	pthread_mutex_lock(&mutex);
	downloads += s_buf.st_size;
	pthread_mutex_unlock(&mutex);
	}

	len = (long)lseek(fd, (off_t)0, SEEK_END);
 	(void)lseek(fd, (off_t)0, SEEK_SET);
  	(void)sprintf(buffer,"HTTP/1.1 200 OK\nServer: 207httpd/%d.0\nContent-len: %ld\nConnection: close\nContent-Type: %s\n\n", VERSION,len, type);

	(void)write(sd,buffer,strlen(buffer));

	while (	(rt = read(fd, buffer, BUFSIZE)) > 0 ) {
	(void)write(sd,buffer,rt);
//printf("%s\n",buffer);

	}
	sleep(1);
	close(sd);
	pthread_exit(NULL);
}

