#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

typedef struct _threadpool_st{
	pthread_t threadinfo;
	pthread_mutex_t waitlock;
	int idle;
} threadpool_item;

#endif
