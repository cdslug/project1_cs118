#include "response.h"

//code found on stackoverflow
//http://stackoverflow.com/questions/7548759/generate-a-date-string-in-http-response-date-format-in-c
//SAMPLE: "Tue, 15 Nov 2010 08:12:31 GMT"
//
//allocate memory in function
//return pointer to cstring
char * dateToStr()
{
  char *buf = malloc(sizeof(char) * (30));
  time_t now = time(0);
  struct tm tm = *gmtime(&now);
  strftime(buf, sizeof buf, "%a, %d %b %Y %H:%M:%S %Z", &tm);
  return buf;
}

char * allocStr(const char * str)
{
	char * buf = NULL;
	buf = strdup(str);
	return buf;
}

//max length of a number is ceiling of log10(2^64-1)+1 = 21
//didn't end up using the method that needed this info
//still seems useful enough to keep
char * numToStr(size_t num)
{
	char buf[21];
	sprintf(buf,"%d",num);
	return allocStr(buf);
}

//the convention I used allocates member strings in helper functions
//the helper functions pass back a pointer to allocated memory
//this memory needs to be cleaned up
void getFileInfo(const http_r * request, http_w * response)
{
	printf("begin gFI\n");
	response->file = malloc(sizeof(file_info));
	memset(response->file,0, sizeof(file_info));

	char * fileBody = NULL;

	FILE *filePointer = NULL;
	filePointer = fopen(&(request->URI[1]), "r");
	printf("in gFI: file opened\n");
	if(filePointer == NULL)
	{
		printf("ERROR, could not open file \"%s\"\n",request->URI);
	}
	else
	{	
		size_t remaining = -1;
		size_t pos = 0;
		size_t fileSize = 0;
		 

		//find file size
		//from stackoverflow
		//http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
		fseek(filePointer, 0L, SEEK_END);
		fileSize = ftell(filePointer);
		fseek(filePointer, 0L, SEEK_SET);
		//end stackoverflow
		printf("in gFI: filesize: %d\n",fileSize);
		remaining = fileSize;

		fileBody = malloc(sizeof(char)*(fileSize+1));
		memset(fileBody, 0, sizeof(char)*(fileSize+1));
		
		//this should concatenate
	    while (fgets (&(fileBody[pos]), remaining, filePointer) != NULL && remaining != 1)
	    {
	    printf("in gFI: fileSize = %d, pos = %d, remaining = %d\n",fileSize, pos, remaining);
	      pos = strlen(fileBody);
	      remaining = fileSize-pos; //because the filesize is one too big
	      
	    }
	    printf("in gFI: fileSize = %d, pos = %d, remaining = %d\n",fileSize, pos, remaining);
	    fclose (filePointer);
	}

		response->file->body = fileBody;
		response->file->type = allocStr("text/html");//todo, based on file extension
		response->file->size = numToStr(strlen(response->file->body));
		
		
}
//allocate all memory in functions before setting response pointers
http_w * generateResponseInfo(http_r * request)
{
	printf("begin gRI\n");
	http_w * response = malloc(sizeof(http_w));

	printf("in gRI: call gFI\n");
	getFileInfo(request, response);

	//assume that if 
	if(response->file->body == NULL)
	{
		response->status = numToStr(404);
		response->status_str = allocStr("Not Found");//not sure if this is standard
	}
	else
	{
		//assume correct
		response->status = numToStr(200);
		response->status_str = allocStr("OK");
	}

	response->HTTP_version = strdup(request->HTTP_version); 
	response->connection = allocStr("close");//todo
	response->date = dateToStr();
	response->server = allocStr("CS118/0.0.1");
	
	return response;
}

char * generateResponseMessage(http_r *request)
{
	printf("begin gRM\n");
	http_w * responseInfo = malloc(sizeof(http_w));
	memset(responseInfo,0,sizeof(http_w));

	char * responseMessage = NULL;
	size_t len = 0;
	printf("in gRM: call gRI\n");
	responseInfo = generateResponseInfo(request);

	len += strlen(responseInfo->HTTP_version);
	len += strlen(responseInfo->status);
	len += strlen(responseInfo->status_str);
	len += strlen(responseInfo->connection);
	len += strlen(responseInfo->date);
	len += strlen(responseInfo->server);
	len += strlen(responseInfo->file->type);
	len += strlen(responseInfo->file->size);
	len += strlen(responseInfo->file->body);
	len += strlen(responseInfo->file->date);

	//using the len above, and calculation below equalling 67, allocate responseMessage
	responseMessage = malloc(sizeof(char) * (len+67+1));
	memset(responseMessage, 0, (len+67+1));
//right now, message does not containresponseInfo->file->date
//6+13+7+9+17+14+1=67
	sprintf(responseMessage, 
"%s %s %s\n\
Connection: %s\n\
Date: %s\n\
Server: %s\n\
Content-Length: %s\n\
Content-Type: %s\n\
\n\
%s",
	responseInfo->HTTP_version,
	responseInfo->status,
	responseInfo->status_str,
	responseInfo->connection,
	responseInfo->date,
	responseInfo->server,
	responseInfo->file->type,
	responseInfo->file->size,
	responseInfo->file->body);

return responseMessage;
}
