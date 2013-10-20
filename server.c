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

#define MAX_FILE_SIZE 1024

void parseClientMessage(char *buffer, int size, char *filePath);
void getFilePath(char *fileLine, int fileSize, char *filePath);
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
     char filePath[255];
     memset(filePath,0,255);
     parseClientMessage(buffer, 255, filePath);
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

//takes in an http request message from the client.
//looks for the line with "Here is the message:"
//returns the name of the file 
//INPUT: c-string of client's request
//OUTPUT: c-string with only the path to the file
//NOTE: this function converts from c-string => string => c-string
void parseClientMessage(char *clientMessage, int size, char *filePath)
{
  //std::string has a constructor that does this?
  char fileLine[255];
  memset(fileLine,0,255);
  //check each line for "Here is the message:"
  int i;

  for(i = 0; i < size; i++){
    if(strncmp(&(clientMessage[i]),"GET ",4) == 0)
    {
      //find the length of the line
      int len = 4;
      while(i+len < size && clientMessage[i+len] != '\n')
      {
        len++;
      }
      // fileLine = clientMessage.substr(i,len);

      strncpy(fileLine,&(clientMessage[i]),len-i);
      getFilePath(fileLine, len-i, filePath);
      break;
    }
    else
    {
      while(i < size && clientMessage[i] != '\n') {i++;}
      i++;//skips the newline and on to the next line
    }
  }
  if(fileLine[0]=='\0')
  {
    error("ERROR pCM: expected \"GET \"");
  }
  
  // std::string filePath = getFilePath(fileLine);
  // c_filePath = filePath.c_str;

}

//takes in a string that begins "Here is the message:"
//looks for requested file name
//return a string with the desired file path
//INPUT: string beginning with "Here is the message:"...
//OUTPUT: string with only the path to the file
void getFilePath(char *fileLine, int size, char *filePath)
{
  //verify that the line starts with "GET "
  memset(filePath,0,size);
  // printf("**size: %d, %s**\n",size, fileLine);
  if(size >= 4 && strncmp(fileLine,"GET ", 4) == 0)
  {
    //start  after the space after 'GET '
    int i;
    for(i = 4; i < size; i++)
    {
      if(fileLine[i] == ' ')
      {
        filePath = strncpy(filePath,&(fileLine[4]),i-4);
        // printf("**pathLen:%d,path:%s**\n",i-4,filePath);
        break;
      }
    }
  }
  else
  {
    error("ERROR gFP: expected \"GET\"");
  }
  
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
