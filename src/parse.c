/*
 * parse.c
 *
 *  Created on: Oct 28, 2013
 *      Author: spencer
 */


#include "parse.h"
#define BUFSIZE 8096


http_r * parseRequest(char* requestMessage) {
	http_r * request = malloc(sizeof(http_r));
	memset(request,0, sizeof(http_r));

	int i;
	if(strncmp(requestMessage,"GET ",4) || strncmp(requestMessage,"get ",4)){
		request->method = malloc(sizeof(char)*5);
		memset(request->method, '\0', sizeof(char)*5);
		strncpy(request->method, requestMessage, 4);
		// printf("in pR, request->method=%s\n",request->method);
	}
	else
	{
		perror("ERROR method not suppported");
		exit(1);
	}

	// printf("in pR, before forloop\n");
	for(i=4; i < BUFSIZE; i++){
		if(requestMessage[i] == ' '){
			request->URI = malloc(sizeof(char)*(i-3));
			memset(request->URI,'\0',sizeof(char)*(i-3));
			strncpy(request->URI,requestMessage+4, i-4);
			break;
		}
		// printf("in pR, requestMessage[%d]=%c\n",i, requestMessage[i]);
	}
	// printf("in pR, after forloop\n");
	if(strlen(request->URI) <= 1 && strcmp(request->URI, "/")) {
		free(request->URI);
		request->URI = strdup("/index.html");
	}

	// printf("in pR, last strcpy\n");
	request->HTTP_version = strdup("HTTP/1.1");
	return request;
}

void freeRequest(http_r* request){
	free(request->HTTP_version);
	free(request->URI);
	free(request->method);
	free(request);
	return;
}
void printRequest(http_r* request) {
	printf("Method: %s\n", request->method);
	printf("URI: %s\n", request->URI);
	printf("HTTP Version: %s\n", request->HTTP_version);

	return;
}

