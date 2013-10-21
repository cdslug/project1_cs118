#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


typedef struct {
	char* method;
	char* request_URI;
	char* HTTP_version;
	char** header_lines;
} http_r;

char** str_split(char* str, const char delim);
http_r* parseRequest(char* requestMessage);
void printRequest(http_r* request);

int main()
{
	char * http_request_message = "GET /somedir/page.html HTTP/1.1\nHost: www.someschool.edu\nAPPLICATION LAYER\nConnection: close\nUser-agent: Mozilla/5.0\nAccept-language: fr";
	http_r* request = parseRequest(http_request_message);
	printRequest(request);

	return 0;
}

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
	
http_r * parseRequest(char* requestMessage) {
	http_r * request = malloc(sizeof(http_r));
	char** request_line;

	request->header_lines = str_split(requestMessage, '\n');
	
	request_line = str_split(request->header_lines[0], ' ');
	request->method = request_line[0];
	request->request_URI = request_line[1];
	request->HTTP_version = request_line[2];
	request->header_lines = &request->header_lines[1];

	return request;
}

void printRequest(http_r* request) {
	printf("Method: %s\n", request->method);
	printf("URI: %s\n", request->request_URI);
	printf("HTTP Version: %s\n", request->HTTP_version);
	printf("Header Lines: \n");
	
	char** tmp = request->header_lines;
	while(tmp != NULL && *tmp) 
		printf("%s\n", *tmp++);
	
	return;
}
