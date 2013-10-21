#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


typedef struct {
	char* method;
	char* URI;
	char* HTTP_version;
	char** header_lines;
} http_r;

char** str_split(char* str, const char delim);
http_r* parseRequest(char* requestMessage);
void printRequest(http_r* request);
