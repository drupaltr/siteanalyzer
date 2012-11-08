#ifndef _NETWORK_H_
#define _NETWORK_H	

typedef struct timeval{
	long tv_sec;
	long tv_usec;
} timeval;
int open_tcp(char *, int);
int nsend(int , const char *, int );
char* nrecv(int );
int recv_line(int , char *);
int close_fd(int );

#endif
