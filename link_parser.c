
/*****
** link_parser.c
** - implements the methods declared in link_parser.h
** - 处理http响应字节流，提取网络链接
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "link_parser.h"

int extract_link(char *htmltxt, char *html_url, urlq_t **url_list_head_ptr)
{
	urlq_t *url_list_head, *url_list_component, *url_list_ptr;
	char *head_ptr, *tail_ptr, *htm_ptr, *href_ptr, *equal,*area,*symbol;
	char *ptr,*q;
	char *url_ptr;
	int  flag = 0;
	int flag2 = 0;
	char *scope;
	int url_ptr_length = 0;
	url_list_head = (urlq_t *)malloc(sizeof(urlq_t));
        url_list_head->next = NULL;

	head_ptr = strstr(htmltxt, "<a");
	while(head_ptr != NULL){
		tail_ptr = strstr(head_ptr,"/a>");
		if(tail_ptr == NULL){
			break;
		}
		href_ptr = strstr(head_ptr, "href");
		/*no href symbol left in the htmltxt*/
		if (href_ptr == NULL){
			break;
		}
		/*no href symbol in current scope*/
		if (tail_ptr - href_ptr < 0 ){
			head_ptr = strstr(tail_ptr,"<a");
			continue;
		}
		equal = href_ptr + 4;
		while (*equal == ' ') equal++;
		if (*equal != '='){
			head_ptr = strstr(tail_ptr, "<a");
			continue;
		}
		htm_ptr = strstr(href_ptr, ".htm");

		if (htm_ptr == NULL){
			break;
		}
		if (tail_ptr - htm_ptr < 0 || htm_ptr - href_ptr > 256){
			head_ptr = strstr(tail_ptr, "<a");
			continue;
		}
		ptr = equal + 1;
		area = strstr(ptr, "<area");
		if (area - ptr > 0 && htm_ptr - area > 0){
			head_ptr = strstr(tail_ptr, "<a");
			continue;
		}
		while(*ptr == ' ')  ptr++;
		if (*ptr == '\'' || *ptr == '\"') ptr++;
		while(*ptr == ' ')  ptr++;
		if(ptr == strstr(ptr, "http://")){
			head_ptr = strstr(tail_ptr, "<a");
			continue;
		}
		
		symbol = strstr(ptr, "\"");
		if (symbol - ptr > 0 && htm_ptr - symbol > 0){
			head_ptr = strstr(tail_ptr, "<a");
			continue;
		}

		if (*(htm_ptr+4) == 'l'){
			url_ptr_length = htm_ptr-ptr+6+strlen(html_url);
			scope = htm_ptr + 5;
		}
		else{
			url_ptr_length = htm_ptr-ptr+5+strlen(html_url);
			scope = htm_ptr + 4;
		}
		url_ptr = (char *)malloc( url_ptr_length * sizeof(char));

		if(url_ptr == NULL)
			return -3;
		strcpy(url_ptr, html_url);
		q = url_ptr + strlen(html_url);
		for(; ptr < scope; ){
			if(*ptr == '.' && *(ptr+1) == '.'){
				if (ptr == url_ptr){
					flag2 = 1;
					break;
				}
				q -= 2;
				while(q - url_ptr >= 0 && *q != '/') q--;
				q++;
				if (q - url_ptr < 0  && *q != '/'){
					flag2 = 1;
					break;
				}
				ptr += 3;
				
				continue;
			}
			*q++ = *ptr;
			ptr++;
		}
		if (flag2 == 1){	
			head_ptr = strstr(tail_ptr, "<a");
			free(url_ptr);
			flag2 = 0;
			continue;
		}
		*q = '\0';
		
		url_list_ptr = url_list_head;
                while (url_list_ptr->next!= NULL){
                        if(strcmp(url_list_ptr->next->url_ptr, url_ptr) == 0){
                               free(url_ptr);
                               flag = 1;
                               break;
                        }
			url_list_ptr = url_list_ptr->next;
                }
		if (flag == 1){	
			head_ptr = strstr(tail_ptr, "<a");
			flag = 0;
			continue;
		}			
                url_list_component = (urlq_t *)malloc(sizeof(urlq_t));
                if (url_list_component == NULL)
                        return -3;
                url_list_component->url_ptr = url_ptr;
                url_list_component->next = NULL;
                url_list_ptr->next = url_list_component;
                head_ptr = strstr(tail_ptr, "<a");
	}
	*url_list_head_ptr = url_list_head;
	return 0;
}
