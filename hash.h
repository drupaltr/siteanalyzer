#ifndef __HASH_H__
#define __HASH_H__
#include <string.h>

typedef struct hash_item{
	int pos;
	struct hash_item *next;
	char *url_ptr;
	int flag;
	int status;
} hash_table;


unsigned int sax_hash(char *);
hash_table *has_url(hash_table *hash, char *);
int insert_hash_item(hash_table *hash, char *,int pos, int flag);
int has_visited(hash_table *hash, char *url);
int set_status(hash_table *hash, char *url_ptr, int status);
#endif
