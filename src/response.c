
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
	strdup(buf, str);
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
char * getFileInfo(const http_r * request, http_w * response)
{
	fileBody = malloc(sizeof(char)*(fileSize+1);
	memset(fileBody, 0, sizeof(char)*(fileSize+1);

	FILE *filePointer = NULL;
	filePointer = fopen(&filePath[1], "r");

	if(filePointer == NULL)
	{
		char temp = malloc(sizeof(char)*(strlen(request->URI)+29);//string should be 29
		sprintf(temp,"ERROR, could not open file \"%s\"\n",filePath);
	    error(temp);
	}
	else
	{	
		size_t  remaining = -1, 
				pos = 0, 
				fileSize = 0;
		char * 	fileBody = NULL;

		//find file size
		//from stackoverflow
		//http://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
		fseek(filePointer, 0L, SEEK_END);
		fileSize = ftell(filePointer);
		fseek(filePointer, 0L, SEEK_SET);
		//end stackoverflow

		remaining = fileSize;

		//this should concatenate
	    while (fgets (&(fileBody[pos]), remaining, filePointer) != NULL )
	    {
	      pos = strlen(fileBody);
	      remaining = fileSize-pos;
	    }
	    fclose (filePointer);
	    return fileBody;
	}

		request->file->body = fileToStr(request->URI);

		request->file = malloc(sizeof(file-info));
		memset(request->file,0, sizeof(file-info));
		request->file->type = allocStr("text/html");//todo, based on file extension
		request->file->size = numToStr(strlen(respone->file->body)));
		
		request->file->date = dateToStr();
}
//allocate all memory in functions before setting response pointers
http_w * generateResponseInfo(http_r * request)
{
	http_w * response = malloc(sizeof(http_w));

	getFileInfo(request, response);

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

	strdup(response->HTTP_version,request->HTTP_version); //look@; not sure if this is right
	response->connection = allocStr("close");//todo
	response->date = dateToStr();
	response->server = allocStr("CS118/0.0.1");
	
	return response;
}

char * generateResponseMessage(http_r *request)
{
	http_w * responseInfo = malloc(sizeof(http_w));
	char * responseMessage = NULL;
	size_t len = 0;

	memset(responseInfo,0,sizeof(http_w));
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

	len += 0;//todo, need to think of an elegant method

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