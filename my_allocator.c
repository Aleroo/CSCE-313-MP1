/* 
    File: my_allocator.c

    Author: Seyed Kasra Rahimi Moghadam
			Alejandro Benevidas
            Department of Computer Science
            Texas A&M University
    Date  : 10/3/2015

    Modified: 

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <math.h>
#include "my_allocator.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

Addr** free_list;

/*--------------------------------------------------------------------------*/
/* GLOBALS */ 
/*--------------------------------------------------------------------------*/

int lists;

Addr start_pointer;
Addr end_pointer;

unsigned int mem_length;

const unsigned int header_size = 13;

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

/* Don't forget to implement "init_allocator" and "release_allocator"! */

unsigned int init_allocator(unsigned int _basic_block_size, unsigned int _length) {
	lists = log2(_length / _basic_block_size) + 1;
	mem_length = _length;
		
	start_pointer = malloc(_length);
	end_pointer = start_pointer - _length;
	
	free_list = malloc(lists * sizeof(Addr**));
	
	int i;
	for (i = 0; i < lists; i ++) {
		free_list[i] = (Addr*)malloc(sizeof(Addr**));
	}
	
	for (i = 0; i < lists; i ++) {
		free_list[i][0] = NULL;
		free_list[i][1] = NULL;
	}
	
	Header* h = start_pointer;
		h->empty = 1;
		h->next = NULL;
		h->prev = NULL;
		h->size = (_length - header_size);
	
	free_list[lists-1][0] = h;	
		
	return h->size;
}

int release_allocator() {
	printf("\nFinal Print:\n\n");
	print();
	free(start_pointer);
}

extern Addr my_malloc(unsigned int _length) {
	Header* h = NULL;
	Header* touse = NULL;
	Header* tNext = NULL;
		
	//printf("\nAllocating [%d]\n",_length);
		
	int i, j;
	int final_i, final_j;
	for (i = lists-1; i >= 0; i --) {
		for (j = 1; j >= 0; j --) {	
			if (free_list[i][j] != NULL) {
				h = free_list[i][j];
				
				if (h->size >= _length) {
					touse = free_list[i][j];
					final_i = i;
					final_j = j;
					break;
				}
			}
		}
	}
			
	if (touse != NULL) {
		int sizeAfterSplit;
		sizeAfterSplit = (touse->size + header_size) / 2 - header_size;
		
		//printf ("ifinal=%d\n",   final_i );
		//printf ("%d\n",  _length && final_i );
		while (sizeAfterSplit >= _length && final_i > 0) {
		//printf ("in %d\n",  _length && final_i );
			sizeAfterSplit = (touse->size + header_size) / 2 - header_size;
				
			//printf("Split %d into",touse->size + header_size);
				
			touse->size = sizeAfterSplit;
			touse->empty = 1;
				
			//printf("[%d][%d]\n",sizeAfterSplit + header_size,sizeAfterSplit + header_size);
				
			Header *hNew = (Addr)(((void*)touse) + (touse->size));
				
			hNew->empty = 1;
			hNew->size = sizeAfterSplit;
				
			tNext = touse->next;
			touse->next = hNew;
			hNew->prev = touse;
			hNew->next = tNext;

			free_list[final_i][final_j] = NULL;
				
			final_i --;
			if (final_j < 1) final_j ++;
			free_list[final_i][0] = touse;
			free_list[final_i][1] = hNew;
													
			touse = hNew;
		}
		
		touse->empty  = 0;
		free_list[final_i][final_j] = NULL;
		
		//print();
		
		return (Addr)touse;
		
	} 
	else {
		printf("Error: Can't fit into memory!\n");
		exit(EXIT_FAILURE);
	}
}

extern int my_free(Addr _a) {
	//Use the code below to print the free_list before each my_free
	//print();
  	Header *h = _a;
  	Header *temp;
  	int l = getList(h);
  	
  	h->empty = 1;
  	
  	if (free_list[l][0] == NULL) {
  		free_list[l][0] = h;
  		//printf("my_free(%d) at [%d][%d]\n",h->size + header_size,l,0);
  	} 
	else if (free_list[l][1] == NULL) {
  		free_list[l][1] = h;
  		//printf("my_free(%d) at [%d][%d]\n",h->size + header_size,l,1);
  	}
  	
  	if (h->prev != NULL) {
	//printf ("Prev\n");
  		temp = h->prev;
		//printf ("%d\n", temp->size);
  		
  		if (temp->empty == 1 && temp->size == h->size) {
  			
  			temp->size = (h->size + header_size)*2 - header_size;
  			temp->next = h->next;
  			if (h->next != NULL) h->next->prev = temp;
  			temp->empty = 1;
  			
  			free_list[l][0] = NULL;
  			free_list[l][1] = NULL;
  			
  			//printf("Merge with (%d)\n", h->size + header_size);
  			my_free((Addr*)temp);
  		}
  	}	 	
  	
  	//printf("Done\n");
}

int getList(Addr _a) {
	Header *a = _a;
	
	int i = lists;
	int size = a->size + header_size;
	
	while (size != mem_length/(int)pow(2,i)) {
		i --;
		if (i < 0) {
			printf("Error: getList failed on %d!",a->size + header_size);
			exit(EXIT_FAILURE);
		}
	}
	
	return lists - i - 1;
}

char * getEmpty (int n){
	if (n == 1){
		return "Empty";
	}
	else if (n == 0){
		return "Full";
	}
}

extern void print() {
	int i, j;
	Header *h = NULL;
	
	printf("Free List:\n");
	for (i = lists-1; i >= 0; i --) {
	
		printf("%d: ",i);
		for (j = 0; j <= 1; j ++) {
			if (free_list[i][j] == NULL) {
				printf("(   )");
			} 
			else {
				h = free_list[i][j];
				printf("(%d)",h->size + header_size);
			}
		}
		
		printf("\n");
	}
	
	//Use below code to see the memory structure after each allocation
	/*printf("\nMemory Structure: \n");
	h = start_pointer;

	while(h != NULL) {
		printf("[");
		if (h->prev != NULL) printf("%d<-", h->prev->size + header_size);
		printf("(%d,%s)",h->size + header_size, getEmpty(h->empty));
		if (h->next != NULL) printf("->%d", h->next->size + header_size);
		printf("] ");
		
		h = h->next;
	}*/
	
	printf("\n\n");
}
	

