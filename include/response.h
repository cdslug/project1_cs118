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
void getFileInfo(const http_r * request, http_w * response);
http_w * generateResponse(http_r * request);
char * generateResponseMessage(http_r *request);

#endif //RESPONSE_H