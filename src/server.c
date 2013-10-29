/*
 * server.c
 *
 *  Created on: Oct 28, 2013
 *      Author: spencer
 */


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
#include "include/parse.h"
#include "include/response.h"

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
     int sockfd, portno; //sockfd:portno -> server socket
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

     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
              error("ERROR on binding");

     listen(sockfd,10);	//5 simultaneous connection at most

     /*//accept connections
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

     if (newsockfd < 0)
       error("ERROR on accept");*/


     fd_set active_fd_set; //set for holding sockets
     int newsockfd; //socket representing client

     /* Initialize the set of active sockets */
     FD_ZERO(&active_fd_set);
     FD_SET(sockfd, &active_fd_set); //put sock to the set to monitor new connections

     while(1){
    	 if(select(FD_SETSIZE,&active_fd_set,NULL,NULL,NULL)<0){exit(-1);}//FAIL with exit status 1 on error


   		 if(FD_ISSET(sockfd, &active_fd_set))//new connection request
	     {
	         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	         FD_SET(newsockfd, &active_fd_set);
	     }
	     if(FD_ISSET(newsockfd, &active_fd_set)){
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
	 		 char * responseMessage = generateResponseMessage(request);

	 		 //reply to client
	 		 n = write(newsockfd,responseMessage,strlen(responseMessage));
	 		 if (n < 0) error("ERROR writing to socket");
	 		 close(newsockfd);//close connection
	 	}
     }
     close(sockfd);
     return 0;
}
