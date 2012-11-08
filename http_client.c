/*****
** http_client.c
** - implements the methods declared in http_client.h
** - 封装处理http协议
*/

#include <stdio.h>
#include <string.h>
#include "network.h"
#include "http_client.h"

int http_do_get(int clientfd,char *root, char *hostname, char *path)
{
	char req[MAXBUFSIZE];

	/*sprintf(req, "HEAD %s HTTP/1.1\r\n", path);*/	
	sprintf(req, "GET /%s%s HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-cn\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\nHost:%s\r\nConnection: Close\r\n\r\n", root, path, hostname);
	/*	strcat(req, "ACCEPT:text/html\r\n");
	sprintf(req+strlen(req), "HOST: %s \r\n", hostname);*/
	return nsend(clientfd, req, strlen(req));
}
int http_response_status(char *buf)
{
	return *(buf + 9) - '0';
}
char* http_response_body(int clientfd)
{
	return  nrecv(clientfd);
}
