#ifndef _WEBGRAPH_H_
#define _WEBGRAPH_H_

#include "hash.h"
#ifndef MAX_VERTEX
#define MAX_VERTEX 400000
#endif

#ifndef MAX_URL
#define MAX_URL 256
#endif

#ifndef Q
#define Q  0.85
#endif

#ifndef E
#define E 0.0001
#endif

typedef struct edge{
	hash_table *hash_ptr;
        struct edge *next;	
} edge;

typedef struct vertex{
	char *url_ptr;
	edge *next;
	int status;
	int total_edge_in;
	int total_edge_out;
} vertex;

typedef struct graph{
	vertex vertex_table[MAX_VERTEX];
	int total_vertex;
} url_web;

url_web * init_webg();
int insert_vertex(url_web *,hash_table *, char *);
int has_vertex(url_web *, char *);
int insert_edge(url_web *, hash_table * ,hash_table *);
int calc_ind_cdf(url_web *, int );
int calc_pagerank(url_web *, int );
int set_webg_status(url_web *, hash_table *, int );
/*int find_ind(url_web *, char *);*/
int gen_graphviz(url_web *);
int destroy_webg(url_web *);

#endif
