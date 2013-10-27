/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <strings.h>
#include <sys/wait.h>	/* for the waitpid() system call */
#include <signal.h>	/* signal name macros, and the kill() prototype */
#include <string.h>
#include <assert.h>
#include <time.h>
#include "parse.h"
#include "response.h"

#define MAX_FILE_SIZE 1024

// void generateResponseMessage(char *filePath, int size, char *responseMessage);
// void fileToMessage(char *filePath, char *fileMessage);

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);	//create socket
     if (sockfd < 0) 
        error("ERROR opening socket");
     memset((char *) &serv_addr, 0, sizeof(serv_addr));	//reset memory
     //fill in address info
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     
     listen(sockfd,5);	//5 simultaneous connection at most
     
     //accept connections
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         
     if (newsockfd < 0) 
       error("ERROR on accept");
         
     int n;
     char buffer[256];
   			 
     memset(buffer, 0, 256);	//reset memory
      
     //read client's message
     n = read(newsockfd,buffer,255);
     if (n < 0) error("ERROR reading from socket");
     printf("Here is the message: %s\n",buffer);

     //parse message here
     http_r* request;
     request = parseRequest(buffer);
	 char* filePath = request->URI;
     printf("Here is the file path: %s\n", filePath);
     ////old code
     // char responseMessage[255+MAX_FILE_SIZE];
     // memset(responseMessage,0,255+MAX_FILE_SIZE);
     // generateResponseMessage(filePath,255,responseMessage);
     char * responseMessage = generateResponseMessage(request);

   	 //reply to client
   	 n = write(newsockfd,responseMessage,strlen(responseMessage));
   	 if (n < 0) error("ERROR writing to socket");
         
     
     close(newsockfd);//close connection 
     close(sockfd);
         
     return 0; 
}

/*
void generateResponseMessage(char *filePath, int size, char *responseMessage)
{
    char fileMessage[MAX_FILE_SIZE];
    int temp = MAX_FILE_SIZE;
    int outputSize = 0;

    memset(fileMessage,0,MAX_FILE_SIZE);
    fileToMessage(filePath, fileMessage);
    outputSize =  sprintf(responseMessage,
"HTTP/1.1 200 OK\n\
Connection: close\n\
Date: \n\
Server: CS118/0.0.1\n\
Content-Length: %d\n\
Content-Type: %s\n\
\n\
%s",
(int)strlen(fileMessage), "text/html", fileMessage);
    printf("**gRM, outputSize=%d,%s\n",outputSize,responseMessage);
}

//SPECIAL CASE: when the file path is empty, open index.html
//WARNING: need to deal with any file size, but right now it's confined to 1024 bytes
void fileToMessage(char *filePath, char *fileMessage)
{
  FILE *pFile;
  pFile = fopen(&filePath[1], "r");
  char temp[50],fileLine[MAX_FILE_SIZE];

  memset(temp,0,50);
  memset(fileLine,0,MAX_FILE_SIZE);
  memset(fileMessage,0,MAX_FILE_SIZE);
  if(pFile == NULL)
  {
    sprintf(temp,"ERROR, could not open file \"%s\"\n",filePath);
    error(temp);
  }
  else
  {
    while (fgets (fileLine, MAX_FILE_SIZE, pFile) != NULL )
    {
      strncat(fileMessage,fileLine,MAX_FILE_SIZE-strlen(fileMessage));
    }
    fclose (pFile);
  }
}
*/
