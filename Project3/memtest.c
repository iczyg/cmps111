#include <stdlib.h> 
#include <stdio.h> 
#include "slug_mem.h"


/* needs to test the following functionality:
	* regular allocation & freeing
	* allocating size 0
	* allocating more than 128mb
	* memstats
*/
int main(void){

	/* regular allocation, no errors expected */
	int *test1 = malloc(sizeof(int));

	/* allocating zero space, expect message to stderr */
	size_t empty = (size_t) 0;
	void *test2 = malloc(empty);

	/* allocating >= 128mb, should print to stderr and end */
	size_t too_much = MAX_ALLOC_SIZE;
	void *test3 = malloc(too_much);

	/* shouldn't be printed */
	printf("don't print me\n");
}
