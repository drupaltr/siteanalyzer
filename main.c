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
#include <unistd.h>
#include "threadpool.h"
#include "hash.h"
#define MAX_THREADS  100

threadpool_item tpool[MAX_THREADS];

pthread_cond_t ready = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

url_web *webg;
queue urlqueue;
hash_table hash[MAX_VERTEX];
char rootdir[] = "techqq/";

int main(void)
{
	int i = 0;
	int in = 0;
	int *out;
	edge *p = NULL;
	int total_found = 0;
	urlq_t *first_node = (urlq_t *)malloc(sizeof(urlq_t));
	first_node->url_ptr = "index.html";
	first_node->next = NULL;
	/*initialize url_queue*/
	queue_init(&urlqueue);
	/*initialize web*/
	webg = init_webg();
	for (i = 0; i < MAX_THREADS; i++){
		tpool[i].idle = 1;
		pthread_mutex_init(&tpool[i].waitlock, NULL);
		pthread_create(&tpool[i].threadinfo, NULL, do_crawler, tpool+i);	
	}
	for (i = 0; i < MAX_VERTEX; i++){
		hash[i].status = 0;
		hash[i].flag = 0;
		hash[i].pos = MAX_VERTEX;
		hash[i].next = NULL;
		hash[i].url_ptr = NULL;
	}

	 /*root url enqueue */ 
        queue_push(&urlqueue, first_node,first_node);
	insert_hash_item(hash, first_node->url_ptr, 0, 0);	
	insert_vertex(webg,has_url(hash, first_node->url_ptr) ,first_node->url_ptr);
		
	pthread_cond_broadcast(&ready);	
	usleep(100);

	while (1){
		pthread_mutex_lock(&mutex);
		if (urlqueue.head_ptr->next != NULL){	
	        	pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&ready);
			usleep(100);
			continue;
		}
		else{
			for (i = MAX_THREADS-1; i >= 0 ; i--){	
				if (pthread_mutex_trylock(&tpool[i].waitlock) != 0){
					i++;
					for (; i < MAX_THREADS; i++)
						pthread_mutex_unlock(&tpool[i].waitlock);
					break;
				}
			/*	pthread_mutex_lock(&tpool[i].waitlock);*/
				if (tpool[i].idle == 0){
					for (; i < MAX_THREADS; i++)
						pthread_mutex_unlock(&tpool[i].waitlock);
					break;
				}
			}
			if(i < 0 ){
				printf("everything is ok");
				break;
			}
	        	pthread_mutex_unlock(&mutex);
			continue;
		}
	}
	out = (int *)malloc(MAX_VERTEX*sizeof(int));
	if (out == NULL)
		return -1;
	for (i = 0; i < MAX_VERTEX; i++)
		*(out + i) = 0;
	total_found = 0;
	for (i = 0; i < webg->total_vertex; i++){
		if(webg->vertex_table[i].status == 0){
			total_found++;		
			p = webg->vertex_table[i].next;
			in = 0;
			while(p != NULL){
				in++;
				out[p->hash_ptr->pos]++;
				p = p->next;
			}
		}
		webg->vertex_table[i].total_edge_in = in; 
		in = 0;
	}
	for (i = 0 ; i < MAX_VERTEX; i++){
		webg->vertex_table[i].total_edge_out = *(out + i);
	}
	free(out);
	gen_graphviz(webg);
	calc_ind_cdf(webg, total_found);
	calc_pagerank(webg, total_found);
	return 0;
}
