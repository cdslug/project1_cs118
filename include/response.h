#ifndef RESPONSE_H
#define RESPONSE_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "parse.h"

typedef struct {
	char* type;
	char* size;
	char* body;
	char* date;
} file_info;

typedef struct {
	char* HTTP_version;
	char* status;
	char* status_str;
	char* connection;
	char* date;
	char* server;
	file_info * file;
} http_w;

char * dateToStr();
char * allocStr(const char * str);
char * numToStr(size_t num);
char * getContentType(const char * URI);
void getFileInfo(const http_r * request, http_w * response);
http_w * generateResponseInfo(http_r * request);
char * generateResponseMessage(http_r * request);
void freeResponse(http_w * response);

#endif //RESPONSE_H