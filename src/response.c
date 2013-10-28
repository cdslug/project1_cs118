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
  printf("in dateToStr: date=%s\n",buf);
  return buf;
}

//max length of a number is ceiling of log10(2^64-1)+1 = 21
//didn't end up using the method that needed this info
//still seems useful enough to keep
char * numToStr(size_t num)
{
	char buf[21];
	sprintf(buf,"%d",num);
	return strdup(buf);
}

char * getContentType(const char * URI)
{
	char * contentType = NULL;
	char * ext = strrchr(URI,'.');

	if(strcmp(ext,".html") == 0)
	{
		contentType = strdup("text/html");
	}
	else if (strcmp(ext, ".css") == 0)
	{
		contentType = strdup("text/css");
	}
	else if (strcmp(ext,".jpg") == 0 || strcmp(ext,".jpeg") == 0)
	{
		contentType = strdup("image/html");
	}
	else if (strcmp(ext,".gif") == 0)
	{
		contentType = strdup("image/html");
	}
	else
	{
		//todo, I don't know, maybe set a status message
		contentType = strdup("text/html");
	}
	return contentType;
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
		int remaining = -1;
		int pos = 0;
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
		//remaining+1 because I don't think the filesize does not include terminating char
	    while (fgets (&(fileBody[pos]), remaining+1, filePointer) != NULL && remaining > 0)
	    {
	    	printf("in gFI: fileSize = %d, pos = %d, remaining = %d\n",fileSize, pos, remaining);
	      	pos = strlen(fileBody);
	      	remaining = fileSize-pos; 
	      
	    }
	    printf("in gFI: fileSize = %d, pos = %d, remaining = %d\n",fileSize, pos, remaining);
	    fclose (filePointer);
	}
	printf("in gFI: fileBody:\n%s\n***END BODY***\n", fileBody);
	printf("in gFI: strlen(fileBody) = %d\n", strlen(fileBody));
		response->file->body = fileBody;
		response->file->type = getContentType(request->URI);
		response->file->size = numToStr(strlen(response->file->body));
		
		
}
//allocate all memory in functions before setting response pointers
http_w * generateResponseInfo(http_r * request)
{
	printf("begin gRI\n");
	http_w * response = malloc(sizeof(http_w));
	memset(response, 0, sizeof(http_w));

	printf("in gRI: call gFI\n");
	getFileInfo(request, response);

	//assume that if 
	if(response->file->body == NULL)
	{
		response->status = numToStr(404);
		response->status_str = strdup("Not Found");//not sure if this is standard
		printf("in gRI: body unfilled\n");
	}
	else
	{
		//assume correct
		response->status = numToStr(200);
		response->status_str = strdup("OK");
		printf("in gRI: body filled, status=%s, stat_str=%s\n",response->status, response->status_str);
	}

	response->HTTP_version = strdup(request->HTTP_version); 
	response->connection = strdup("close");//todo
	response->date = dateToStr();
	response->server = strdup("CS118/0.0.1");
	printf("in gRI: response status = %s\n",response->status);
	return response;
	printf("in gRI: wARNING, after response somehow\n");
}

char * generateResponseMessage(http_r *request)
{
	printf("begin gRM\n");
	http_w * response = NULL;

	char * responseMessage = NULL;
	size_t len = 0;
	printf("in gRM: call gRI\n");
	response = generateResponseInfo(request);
	printf("in gRM: info returned\n");

	len += strlen(response->HTTP_version);
	len += strlen(response->status);
	len += strlen(response->status_str);
	len += strlen(response->connection);
	len += strlen(response->date);
	len += strlen(response->server);
	len += strlen(response->file->type);
	len += strlen(response->file->size);
	len += strlen(response->file->body);
	////if no date is allocated in getFileInfo, then this crashes the program
	// len += strlen(response->file->date);

	printf("in gRM: info length=%d\n",len);
	//using the len above, and calculation below equalling 67, allocate responseMessage
	responseMessage = malloc(sizeof(char) * (len+67+1));
	memset(responseMessage, 0, (len+67+1));
//right now, message does not containresponse->file->date
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
	response->HTTP_version,
	response->status,
	response->status_str,
	response->connection,
	response->date,
	response->server,
	response->file->size,
	response->file->type,
	response->file->body);
printf("in gRM: responseMessage:\n%s\n***END RESPONSE MESSAGE***\n", responseMessage);


//free the allocated space in the response variable
freeResponse(response);

return responseMessage;
}

void freeResponse(http_w * response)
{
	//would have been more elegant if I used a string array
	free(response->file->size);
	free(response->file->type);
	free(response->file->body);
	free(response->file);

	free(response->HTTP_version);
	free(response->status);
	free(response->status_str);
	free(response->connection);
	free(response->date);
	free(response->server);
	free(response);
}
