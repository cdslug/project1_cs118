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

#define MAX_FILE_SIZE 1024

typedef struct {
	char* method;
	char* URI;
	char* HTTP_version;
	char** header_lines;
} http_r;

char** str_split(char* str, const char delim);
http_r* parseRequest(char* requestMessage);
void generateResponseMessage(char *filePath, int size, char *responseMessage);
void fileToMessage(char *filePath, char *fileMessage);

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

     char responseMessage[255+MAX_FILE_SIZE];
     memset(responseMessage,0,255+MAX_FILE_SIZE);
     generateResponseMessage(filePath,255,responseMessage);
   	 //reply to client
   	 n = write(newsockfd,responseMessage,strlen(responseMessage));
   	 if (n < 0) error("ERROR writing to socket");
         
     
     close(newsockfd);//close connection 
     close(sockfd);
         
     return 0; 
}

//takes in an http request message from the client (assumes HTTP GET).
//splits the message into multiple parts and fills in http_r struct
//returns the struct of http request 
//INPUT: c-string of client's request
//OUTPUT: http_r struct
http_r * parseRequest(char* requestMessage) {
	http_r * request = malloc(sizeof(http_r));
	char** request_line;

	request->header_lines = str_split(requestMessage, '\n');
	
	request_line = str_split(request->header_lines[0], ' ');
	request->method = request_line[0];
	request->URI = request_line[1];
	request->HTTP_version = request_line[2];
	request->header_lines = &request->header_lines[1];

	return request;
}

// Splits a string into several tokenized strings separated by a delimiter character
// INPUT: c-string , constant delimiter character
// OUTPUT: c-string array of tokenized string
char** str_split(char* str, const char delim)
{
	char** result	= 0;
	size_t count	= 0;
	char* tmp	= str;
	char* last_delim= 0;

	// Count elements to be extracted
	while (*tmp)
	{
		if(delim == *tmp){
			count++;
			last_delim = tmp;
		}
		tmp++;
	}
	
	// Add space for trailing token
	count += last_delim < (str + strlen(str)-1);
	// Allocate memory for result and add space for NULL pointer
	result = malloc(sizeof(char*) * (count+1));

	if(result)
	{
		size_t i = 0;
		char* copy = strdup(str);
		const char delim_s[2] = {delim,'\0'};
		char* token = strtok(copy, delim_s);
	
		while (token)
		{
			assert(i < count);
			*(result + i++) = strdup(token);
			token = strtok(NULL, delim_s);
		}
		assert(i == count);
		*(result+i) = NULL;
	}
	
	return result;
}

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
