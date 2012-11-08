#include <stdio.h>
#include <stdlib.h>
#include "webgraph.h"
#include "hash.h"

unsigned int sax_hash(char *key)
{
	unsigned int h=0;

	while(*key)
        h^=(h << 5)+(h >> 2)+(unsigned char)(*key++);

	return h;
}

hash_table *has_url(hash_table *hash, char *url)
{
	unsigned int pos;
	hash_table *p;
	pos = sax_hash(url) % MAX_VERTEX;
	p = hash+pos;
	if (p->url_ptr != NULL){
		if(strcmp(p->url_ptr, url) == 0){
			return p;
		}
	}
	p = p->next;
	while(p != NULL){
		if(strcmp(p->url_ptr, url) == 0){
			return p;
		}
		p = p->next;
	}
	return NULL;
}

int has_visited(hash_table *hash, char *url)
{
	unsigned int pos;
	hash_table *p;
	pos = sax_hash(url) % MAX_VERTEX;
	p = hash+pos;
	if (p->url_ptr != NULL && strcmp(p->url_ptr, url) == 0){
		if(p->flag == 0){
			p->flag = 1;
			return 0;
		}
		else{
			return 1;
		}
	}
	p = p->next;
	while(p != NULL){
		if(strcmp(p->url_ptr, url) == 0){
			if(p->flag == 0){
				p->flag = 1;
				return 0;
			}
			else{
				return 1;
			}
		}
		p = p->next;
	}
	return -1;
}
int insert_hash_item(hash_table *hash, char *url, int pos, int flag)
{
	hash_table *item, *p;
	unsigned int position = sax_hash(url) % MAX_VERTEX; 
	p = hash + position;
	if (p->url_ptr == NULL){
		p->pos = pos;
		p->url_ptr = url;
		p->flag = flag;
		return 0;
	}
	item = (hash_table *)malloc(sizeof(hash_table));
	item->status = 0;
	item->next = p->next;
	item->flag = flag;
	item->url_ptr = url;
	item->pos = pos;
	p->next = item;
	return 0;
}
int set_status(hash_table *hash, char *url_ptr, int status)
{
	unsigned int pos;
	hash_table *p;
	pos = sax_hash(url_ptr) % MAX_VERTEX;
	p = hash+pos;
	if (p->url_ptr != NULL && strcmp(p->url_ptr, url_ptr) == 0){
		p->status = 4;
		return 1;
	}
	p = p->next;
	while(p != NULL){
		if(strcmp(p->url_ptr, url_ptr) == 0){
			p->status = 4;	
			return 1;
		}
		p = p->next;
	}
	return -1;
	
}
