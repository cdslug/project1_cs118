#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
	char* HTTP_version;
	char* status;
	char* status_str
	char* connection;
	char* date;
	char* server;
	file_info * file;
} http_w;

typedef struct {
	char* type;
	char* size;
	char* body;
	char* date;
} file_info;

char * dateToStr();
char * allocStr(const char * str);
char * numToStr(size_t num);
char * getFileInfo(const http_r * request, http_w * response);
http_w * generateResponse(const http_r * request);
char * generateResponseMessage(const http_w *responseInfo);