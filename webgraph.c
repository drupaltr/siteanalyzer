/*****
** webgraph.c
** - implements the methods declared in webgraph.h
** - 构造、维护和存储整个web graph的顶点和边数据,分析web graph的链接关系,确保多线程安全访问
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "webgraph.h"

url_web *init_webg()
{
	url_web *webg;
	int i = 0;

	webg = (url_web *)malloc(sizeof(url_web));
	if(webg == NULL)
		return NULL;
	for (i = 0; i < MAX_VERTEX; i++){
		webg->vertex_table[i].next = NULL;
		webg->vertex_table[i].url_ptr = NULL;
		webg->vertex_table[i].total_edge_out = 0;
		webg->vertex_table[i].total_edge_in = 0;
		webg->vertex_table[i].status = 0;
	}
	webg->total_vertex = 0;
	return webg;
}
int insert_vertex(url_web *webg, hash_table *hash_out, char *url)
{
	edge *insert;
        
	insert	= (edge *) malloc(sizeof(edge));
	if (insert == NULL) return -3;

	insert->hash_ptr = hash_out;
	insert->next = webg->vertex_table[webg->total_vertex].next;
	
	webg->vertex_table[webg->total_vertex].url_ptr = url;
	webg->vertex_table[webg->total_vertex].next = insert;
	webg->total_vertex++;
	return (webg->total_vertex - 1);
}

int set_webg_status(url_web *webg, hash_table *hash_out, int flag)
{
	webg->vertex_table[hash_out->pos].status = flag;
	return 0;
}

int insert_edge(url_web *webg, hash_table *hash_in, hash_table *hash_out)
{
	edge *insert;
        
	insert	= (edge *) malloc(sizeof(edge));
	if (insert == NULL) return -3;

	insert->hash_ptr = hash_out;
	insert->next = webg->vertex_table[hash_in->pos].next;
	
	webg->vertex_table[hash_in->pos].next = insert;	
	return 0;
}
int calc_pagerank(url_web *webg, int total)
{
	int i,flag, j, pos;
	double cur;
	edge *ptr;
	vertex *ver;
	int pagerank[160000];
	float *temp;
	float *ri = (float *)malloc(webg->total_vertex * sizeof(float));
	float *rj = (float *)malloc(webg->total_vertex * sizeof(float));

	FILE *fp = NULL;

	if(ri == NULL || rj == NULL)
		return -1;   

	for (i = 0; i < webg->total_vertex; i++)
		if(webg->vertex_table[i].status == 0)
			*(rj + i) = 1.0;
		else
			*(rj + i) = -1;

	do{
		flag = 0;
		temp = rj;
		rj = ri;
		ri = temp;
		for(i = 0; i < webg->total_vertex; i++){
			cur = 0.0;
			ver = webg->vertex_table + i;
			if (ver->status == 0){
				ptr = webg->vertex_table[i].next;
				while(ptr != NULL){
					cur += *(ri+ptr->hash_ptr->pos);
					ptr = ptr->next;
				}
		       		 *(rj + i) =(1-Q)/(float)total + Q * cur / webg->vertex_table[i].total_edge_out;
				if(((*(rj+i)-*(ri+i)) > E) || (*(rj+i)-*(ri+i) < -E))
					flag = 1;
			}
		}				
	}while(flag);
	free(ri);
	pagerank[0] = MAX_VERTEX+1;
	pos = 1;
	for (i = 0; i < webg->total_vertex; i++){
		if(webg->vertex_table[i].status == 0){
			j = pos-1;
			while(j > 0 && *(rj + pagerank[j]) < *(rj+i) ){
				pagerank[j] = pagerank[j+1];
				j--;
			}
			pagerank[j+1] = i;
			pos++;
		}
	}
	if ((fp = fopen("top10.dat", "w")) == NULL)
		return -1;
	for (i = 1; i <= 10; i++)
		if(fprintf(fp, "%d\n", (webg->vertex_table + pagerank[i])->total_edge_in) < 0)
			return -2;
	for (i = 1; i <= 10; i++)
		if(fprintf(fp, "%s %f\n",( webg->vertex_table + pagerank[i])->url_ptr, *(rj+pagerank[i])) < 0)
			return -2;
	free(rj);
	fclose(fp);
    	return 0;   
}

int calc_ind_cdf(url_web *webg, int total)
{
	int ind[400000];
	int i = 0;
	int j = 0;
	int max = 0;
	int end = 0;
	int pos = 0;
	int k = 0;
	FILE *fp = NULL;
	int num = 0;
	
	
	if ((fp = fopen("indcdf.dat", "w")) == NULL)
		return -1;

	ind[0] = 0;
	pos = 1;

	

	for (i = 0; i < webg->total_vertex; i++){
		if(webg->vertex_table[i].status == 0){
			j = pos-1;
			while(ind[j] > webg->vertex_table[i].total_edge_in){
				ind[j+1] = ind[j];
				j--;
			}
			ind[j+1] = webg->vertex_table[i].total_edge_in;
			pos++;
		}
	}
	max = ind[pos-1];
	end = pos - 1 ;

	i = 1;
	k = 1;
	num = 0;
	while(k <= max){
		while(ind[i] <= k && i <= end){
			num++;
			i++;
		}
		if (fprintf(fp, "%d %f\n", k, num/(float)total) < 0)
			return -1;
		k++;			
	}
	fclose(fp);
	return 0;
}

int gen_graphviz(url_web *webg){
	int i = 0;
 	edge *p;
	FILE *fp;

	if ((fp = fopen("sitemap.dot", "a+")) == NULL)
		return -1;
	if(fprintf(fp, "%s\n", "digraph sitemap{") < 0)
		return -1;
	for (i = 0; i < webg->total_vertex; i++){
		if (webg->vertex_table[i].status == 0){
			if(fprintf(fp, "%d;\n", i) < 0)
				return -1;	
		}
	}
	if(fprintf(fp, "\n") < 0)
		return -1;
	for (i = 0; i < webg->total_vertex; i++){
		if (webg->vertex_table[i].status == 0){
			p = webg->vertex_table[i].next;
			while(p != NULL){
				if (p->hash_ptr->status == 0)
					if(fprintf(fp, "%d -> %d;\n", p->hash_ptr->pos, i) < 0)
						return -1;
				p = p->next;
			}
		}
	}
	if(fprintf(fp, "}\n") < 0)
				return -1;
	fclose(fp);
	return 0;	
}
int destroy_webg(url_web *webg)
{
	int i = 0;
	struct edge *p , *q;
	for (i = 0; i < webg->total_vertex; i++){
		p = webg->vertex_table[i].next;
		while(p != NULL){
			q = p->next;
			free(p);
			p = q;
		}
		free(webg->vertex_table[i].url_ptr);
	}
	return 0;
}

