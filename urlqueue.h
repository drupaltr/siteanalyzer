#ifndef _URL_QUEUE_H
#define _URL_QUEUE_H

typedef struct url_queue {
	char *url_ptr;
	struct url_queue *next;
} urlq_t;

typedef struct queue{
	urlq_t *head_ptr;
	urlq_t *tail_ptr;
} queue;

/*****
** initialize an empty urlq_t
** must be called first after a new urlq_t is declared
*/ 
int queue_init(queue *);


/*****
** push a new element to the end of the urlq_t
** it's up to the client code to allocate and maintain memory of "element"
*/ 
int queue_push(queue *, urlq_t *, urlq_t *);

/*****
** return the first element in the urlq_t, or NULL when the urlq_t is empty
* 
int queue_front(urlq_t *, char **);
*/
/*****
** remove the first element (pointer) from the urlq_t
** set "release" to non-zero if memory deallocation is desired
*/ 
char *queue_pop(queue *);

/*****
** remove all elements (pointers) from the urlq_t
** set "release" to non-zero if memory deallocation is desired
*/ 
/*int queue_clear(urlq_t *);*/

/*****
** return current number of elements in the urlq_t, or 0 when urlq_t is empty
*/ 
/*int queue_size(urlq_t *);*/

#endif /* _RZ_C_QUEUE_ */

