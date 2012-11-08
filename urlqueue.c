/*****
** urlqueue.c
** - implements the methods declared in urlqueue.h
*/

#include <stdlib.h>
#include "urlqueue.h"

int  queue_init(queue *q)
{
	q->head_ptr = (urlq_t *)malloc(sizeof(urlq_t)) ;
        if(q->head_ptr == NULL)
                return -3;
        q->head_ptr->next = NULL;
        q->head_ptr->url_ptr = NULL;
        q->tail_ptr = q->head_ptr;
        return 0;

}

/*int queue_size(urlq_t *q)
{
	int len = 0;
	while (q->next != NULL){
		len++;
		q = q->next;
	}
	return len;
}*/

int queue_push(queue *q, urlq_t *elem, urlq_t *tail)
{
	q->tail_ptr->next = elem;
	q->tail_ptr = tail;
	return 0;
}

char *queue_pop(queue *q)
{
	char *p;
	urlq_t *member ;
	
	if (q->head_ptr->next == NULL )
		return NULL;
	member = q->head_ptr->next; 
	q->head_ptr->next = member->next;
	p = member->url_ptr;
	free(member);
	if(q->head_ptr->next == NULL)
		q->tail_ptr = q->head_ptr;
	return p;
}
/*
int queue_front(urlq_t *head, char **url_ptr_ptr)
{
	*url_ptr_ptr = head->next->url_ptr;	
	return 0;
}
*/
/*int queue_clear(urlq_t *head)
{
	urlq_t *q;

	while(head->next != NULL){
		q = head;
		head = head->next;
		free(q);		
	}
	return 0;
}*/

