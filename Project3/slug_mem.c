// CREATED



struct _Node {
        int block_size;
	void *address;
	char *caller;
	time_t time_stamp;
	char freed;
	Node next;
	Node prev;
};

struct _Mem {
    Node first;
	Node last;
	Node current;
	int mem_size;
	int num_alloca;  //added this for counter total # of allocations done overall.
	int total_size;  //added for calculating the mean and SD of all allocation
	double mean;
	double SD;
};

Mem memory;

void mem_init (void) {
        memory = malloc(MAX_ALLOC_SIZE);
	memory->first = NULL;
	memory->last = NULL;
	memory->current = NULL;
	memory->mem_size = MAX_ALLOC_SIZE;
	memory->num_alloca = 0;   //added this for counting total # of allocations done overall.
	memory->total_size = 0;
	memory->mean = 0;
	memory->SD = 0;
}

void insert_node (Node node) {
    if (memory->first == NULL) {
	    memory->first = memory->last = node;
	    memory->first->prev = memory->first->next = NULL;
	} else {
		node->prev = memory->last;
		memory->last->next = node;
	        memory->last = node;
	}
	node->next = NULL;
	node->freed = FALSE;
	memory->current = node;
	memory->num_alloca++;  //increase by 1 for each new allocation
	memory->total_size+=node->block_size;
	memory->mean = (double)memory->total_size / memory->num_alloca;
}

int mem_is_valid (void *address, char *caller) {
    memory->current = memory->first;
	while (memory->current != NULL ) {
	    if (memory->current->address == address && memory->current->caller == caller ) {
	    	if(memory->current->freed == FALSE){
		    return TRUE;
	    	}
	    	else{
	    	    return FALSE;
	    	}
		}
	          memory->current = memory->current->next;
	}
	return FALSE;
}

void *slug_malloc (size_t size, char *WHERE) {

    void *data_address;
	time_t allocation_time;

	/* check that the allocation is of legal size */
	if(size >= MAX_ALLOC_SIZE){
		fprintf(stderr, "ERR: Cannot allocate more than 2^27-1 mb \n");
		exit(1);
	}
	
    /* Allocate a node */
    Node new_node = malloc(sizeof(Node));
	/* Handles errors */
	if (!new_node) {
	    fprintf (stderr, "ERR(%s): Cannot create node\n", strerror (errno));
	    return;
    }
	
	/* Allocate memory */
	data_address = malloc(size);
	/* Handles errors */
	if (!data_address) {
	    free(new_node);
	    fprintf (stderr, "ERR(%s): Cannot allocate %d in memory\n", strerror (errno), size);
	    return;
    }
	/* Saves address and size */
        new_node->address = data_address;
	new_node->block_size = size;
	
	/* Create time stamp */
	time(&allocation_time);
	new_node->time_stamp = allocation_time;
	
	/* Saves caller info */
	new_node->caller = WHERE;
	
        /* Insert node */
	insert_node(new_node);
	return data_address;
}

void slug_free ( void *addr, char *WHERE ) {

    /* Check for memory validity, handles pointers, free memory and the node */
    if (mem_is_valid(addr, WHERE)) {
	free(memory->current->address);
	memory->current->freed = TRUE;
    }
}

void slug_memstats ( void ) {
	int Num_active_alloc = 1;
	int total_active_size = 0;
	int difference = 0;
        
	Node node;
	node = memory->first;
	while(node != NULL){
		if(node->freed == FALSE){
			printf("allocation[%d] %s(%11d) - size: %d, address: % p \n", 
		        	Num_active_alloca, node->caller, node->time_stamp, node->block_size, node->address);
			Num_active_alloca++;
			total_active_size += node->block_size;
			node = node->next;
		}
		difference += (pow(node->block_size - memory->mean, 2.0));
	}
	memory->SD = sqrt(difference);

	printf("----------------------------------------------------------\n");
	printf("*****               Allocation Summary               *****\n");
	printf("Total number of allocations done: %d\n", memory->num_alloca);
	printf("Total number of active allocation: %d\n", Num_active_alloca);
	printf("Total memory size of active allocation: %d\n", total_active_size);
	printf("Mean: %f   SD: %f\n",memory->mean, memory->SD);
	printf("----------------------------------------------------------\n");
	
	node = memory->first;
	while(node != NULL){
		if(node->freed == FALSE){
			free(node->address);
		}
		free(node);
		node = node->next;
	}
}
