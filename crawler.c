/******
** crawler.c
** - implements the methods declared in crawler.h
** -爬取线程的核心流程
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "network.h"
#include "http_client.h"
#include "link_parser.h"
#include "urlqueue.h"
#include "webgraph.h"
#include "crawler.h"
#include "threadpool.h"
#include "hash.h"

extern url_web *webg;
extern queue urlqueue;
extern pthread_mutex_t mutex;
extern pthread_cond_t ready;
extern hash_table hash[MAX_VERTEX];
extern char rootdir[10];

void * do_crawler(void *item)
{
	char *url_ptr;
	int clientfd ;
	static int pages = 0;
	static int error = 0;
	char *buf = NULL;
	urlq_t *url_list_head = NULL, *p, *p_pre;
	char cur_dir[256];
	hash_table *hash_in, *hash_out;
/*	int tid = pthread_self();*/
	int j = 0;
	int i= 0;
	int pos_found = 0;
	char temp[256];
	int status = 0;
	static int a = 0;
	static int b = 0;
	static int c = 0;
	static int d = 0;
	static int e = 0;
	static int f = 0;
	static int g = 0;
	while(1){	
		pthread_mutex_lock(&mutex);
		while (urlqueue.head_ptr->next == NULL){
			pthread_cond_wait(&ready, &mutex);
		}

		pthread_mutex_lock(&((threadpool_item *)item)->waitlock);
		((threadpool_item *)item)->idle = 0;
		pthread_mutex_unlock(&((threadpool_item *)item)->waitlock);	
		url_ptr = queue_pop(&urlqueue);
		g++;
		pthread_mutex_unlock(&mutex);
		

		pthread_mutex_lock(&mutex);
		/*if not visited, set flag = 1*/
		if(has_visited(hash, url_ptr) == 1){
			pthread_mutex_unlock(&mutex);
			free(url_ptr); 
			pthread_mutex_lock(&((threadpool_item *)item)->waitlock);	
			((threadpool_item *)item)->idle =1;
			pthread_mutex_unlock(&((threadpool_item *)item)->waitlock);
			d++;	
			continue;	
		}
		hash_out = has_url(hash, url_ptr);
		e++;
		pthread_mutex_unlock(&mutex);
		if (hash_out == NULL){
			printf("error\n");
			getchar();
		}
		*temp = '\0';
		cur_dir[0] = '\0';
		strcpy(cur_dir, url_ptr);
		j = strlen(cur_dir);
		for (;cur_dir[j] != '/' && j != 0; j--) ;
		if(j == 0)
			cur_dir[j] = '\0';
		else
			cur_dir[j+1] = '\0';

		for (i = 0; i < 3; i++){
			if((clientfd = open_tcp("127.0.0.1", 80)) < 0){
				close_fd(clientfd);
				continue;
			}

			if( http_do_get(clientfd, rootdir, "127.0.0.1", url_ptr) < 0){
				close_fd(clientfd);
				continue;
			}

			if(recv_line(clientfd, temp) <= 0){
				close_fd(clientfd);
				continue;
			}
			if((status = http_response_status(temp))  == 4){
				printf("%s error %d\n",url_ptr, error++);
				pthread_mutex_lock(&mutex);
				set_status(hash, url_ptr, 4);
				set_webg_status(webg, hash_out, 4);
				pthread_mutex_unlock(&mutex);
			
				pthread_mutex_lock(&((threadpool_item *)item)->waitlock);	
				((threadpool_item *)item)->idle =1;
				pthread_mutex_unlock(&((threadpool_item *)item)->waitlock);	
				close_fd(clientfd);
				break;	
			}
			buf = http_response_body(clientfd);
			close_fd(clientfd);
			break;
		}
		if (status == 4)
			continue;
		if(i == 3){
			pthread_mutex_lock(&((threadpool_item *)item)->waitlock);	
			((threadpool_item *)item)->idle =1;
			pthread_mutex_unlock(&((threadpool_item *)item)->waitlock);	
			close_fd(clientfd);
			continue;	
		}
		if (buf == NULL){
			pthread_mutex_lock(&((threadpool_item *)item)->waitlock);	
			((threadpool_item *)item)->idle =1;
			pthread_mutex_unlock(&((threadpool_item *)item)->waitlock);	
			continue;
		}
		printf("%s pages %d\n", url_ptr,pages++);
		extract_link(buf, cur_dir, &url_list_head);
		free(buf);
		buf = NULL;	
		p = url_list_head->next;
		p_pre = url_list_head;
		while (p != NULL){
			
			if(strcmp(url_ptr, p->url_ptr) == 0){
				p_pre->next = p->next;
				free(p->url_ptr);
				free(p);
				a++;
				p = p_pre->next;
				printf("a= %d, b= %d, c= %d, d= %d, e= %d, f= %d, g= %d\n", a,b,c,d,e,f,g);
				continue;	
			}			
			pthread_mutex_lock(&mutex);
			hash_in = has_url(hash, p->url_ptr);	
			if (hash_in != NULL ){
				insert_edge(webg, hash_in, hash_out);
				pthread_mutex_unlock(&mutex);
				p_pre->next = p->next;
				free(p->url_ptr);
				free(p);
				p = p_pre->next;
				b++;
				printf("a= %d, b= %d, c= %d, d= %d, e= %d, f= %d, g= %d\n", a,b,c,d,e,f,g);
				continue;
			}
			else{
				pos_found = insert_vertex(webg, hash_out, p->url_ptr);
				insert_hash_item(hash, p->url_ptr, pos_found, 0);	
				pthread_mutex_unlock(&mutex);
				c++;
				p_pre = p;
				p = p->next;
				printf("a= %d, b= %d, c= %d, d= %d, e= %d, f= %d, g= %d\n", a,b,c,d,e,f,g);
			}
		}
		
		if(p_pre != url_list_head){
			pthread_mutex_lock(&mutex);
			queue_push(&urlqueue, url_list_head->next, p_pre);
			f++;
			pthread_mutex_unlock(&mutex);
		}
		free(url_list_head);
		p = p_pre = url_list_head = NULL;
	
		pthread_mutex_lock(&((threadpool_item *)item)->waitlock);	
		((threadpool_item *)item)->idle = 1;
		pthread_mutex_unlock(&((threadpool_item *)item)->waitlock);	
/*printf("next time!\n");*/
	}
/*printf("over!\n");*/
	return NULL;	
}
