/*****
** network.c
** - implements the methods declared in network.h
** - ¶ÔÍøÂçsocketµÄÊÕ·¢½øÐÐ·â×°
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "network.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int open_tcp(char *hostname, int port)
{
	int clientfd;
	struct hostent *hp;
	struct sockaddr_in serveraddr;
/*	int nRecvBuf = 16 * 1024; */
	int nNetTimeout = 1000; /*1秒*/
	/*timeval tv = {10, 0};*/

/*	printf("tcp_get in\n");*/
	if((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;
/*	setsockopt(clientfd, SOL_SOCKET, SO_RCVBUF, (const char *) &nRecvBuf, sizeof(int));*/
        setsockopt(clientfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&nNetTimeout, sizeof(int));
/*	printf("get the host ip address\n");*/
	if((hp = gethostbyname(hostname)) == NULL)/*取得主机ip地址*/
		return -3;

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(port);
	bcopy((char *)hp->h_addr_list[0],(char *)&serveraddr.sin_addr.s_addr, hp->h_length);
/*	printf("connect get ready!\n");*/
	if(connect(clientfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
		return -4;
/*	printf("leave tcp_open\n");*/
	return clientfd;
}

int nsend(int clientfd, const char *msg, int len) 
{
    	int nleft = len;
	int nsend = 0;
	const char *ptr = msg;

	while(nleft > 0)
	{ 
		if((nsend = send(clientfd, ptr, nleft, 0)) < 0)
			return -1;
		else if(nsend == 0)
			break;
		
		nleft -= nsend;
		ptr += nsend;
	}
/*	printf("%d bytes send!\n", len-nleft);*/
	return len-nleft;
}

char* nrecv(int clientfd)
{
	int read = 0, nread = 0;
	char head_line[100];
	char content_len[] = "Content-Length: ";
	char *p = NULL;
	char *buf = NULL;
	int body_len = 0;
/*printf("step into nrecv!\n");*/
	do{
		if (recv_line(clientfd,head_line) <= 0)
			return NULL;
	}while(strstr(head_line, content_len) == NULL);

	p = head_line + 16;
	while(*p != '\0'){
	       	body_len = body_len * 10 + *p - '0';
		p++;
	}
/*printf("nrecv 85\n");*/
	do{
		if(recv_line(clientfd, head_line) < 0)
			return NULL;
		if(strlen(head_line) == 0)
			break;
	}while(1);
/*printf("nrecv 92\n");*/
	if((buf = (char *) malloc((body_len+1) * sizeof(char))) == NULL)
		return NULL;
	do{
		read = recv(clientfd, buf + nread, body_len-nread, 0);
		if(read < 0){
			return NULL;
		}
		nread += read;
	}while(read > 0);
/*printf("nrecv 102\n");*/
	*(buf+body_len) = '\0';
/*	printf("will out the nrecv\n");*/
	return buf;
}


int recv_line(int clientfd, char *buf)
{
	char temp;
	int read = 0;
	int i = 0;
/*	printf("step into readline!\n");*/
	do{
		read = recv(clientfd, &temp, 1, 0);
/*		printf("recv_line read = %d\n", read);*/
		if(read == 1 && temp != '\n')
			buf[i++] = temp;
		else if(temp == '\n')
			break;
	}while(read > 0);
/*	printf("leave recv_line\n");*/
	if(i != 0){
		buf[i-1] = '\0';
		return i-1;
	}
	else{
		return -1;
	}
}



int close_fd(int clientfd)/*¹Ø±Õsocket*/
{
   if( close(clientfd) != 0)
	return -1;
   return 0;
}


