#ifndef _HTTPCLIENT_H_
#define _HTTPCLIENT_H_

#ifndef MAXBUFSIZE
#define MAXBUFSIZE 2048
#endif

int http_do_get(int ,char *,char *, char *);
int http_response_status(char *);
char* http_response_body(int );

#endif
