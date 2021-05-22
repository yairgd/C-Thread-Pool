/* 
 * WHAT THIS EXAMPLE DOES
 * 
 * We create a pool of 4 threads and then add 40 tasks to the pool(20 task1 
 * functions and 20 task2 functions). task1 and task2 simply print which thread is running them.
 * 
 * As soon as we add the tasks to the pool, the threads will run them. It can happen that 
 * you see a single thread running all the tasks (highly unlikely). It is up the OS to
 * decide which thread will run what. So it is not an error of the thread pool but rather
 * a decision of the OS.
 * 
 * */

#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include "thpool.h"
#include "mymalloc.h"
#include <kernel.h>

static char __aligned(4) my_slab_buffer[10 * 64];
static struct k_mem_slab my_slab;

K_HEAP_DEFINE(heap, 4096);


void * malloc_get(int size) {
#if 0
	char *block_ptr;
	k_timeout_t timeout;
	timeout.ticks=100;
	if (k_mem_slab_alloc(&my_slab, &block_ptr, timeout) == 0) {
		memset(block_ptr, 0, 64);
	} else {
		printf("Memory allocation time-out");
	}
	return block_ptr;
#endif
	 char *mem_ptr=0;
alloc:
	mem_ptr =  k_malloc(size);
	if (mem_ptr != NULL) {
		memset(mem_ptr, 0, size);
	} else {
		printf("Memory not allocated");
	//	goto alloc;
	}

	return mem_ptr;
}

void  malloc_put(void *block_ptr) {

	//k_mem_slab_free(&my_slab, &block_ptr);
	k_free(block_ptr);

}
void  malloc_init() {
//	k_mem_slab_init(&my_slab, my_slab_buffer, 512, 20);	
}


void task(void *arg){
	printk("Thread #%u working on %d\n", (int)pthread_self(), (int) arg);
	k_msleep(1000);
}


//gcc mymalloc.c example.c  thpool.c -o ex -lpthread -Wno-pointer-to-int-cast -g -O0
int main(){

	malloc_init();

	
	puts("Making threadpool with 4 threads");

	threadpool thpool = thpool_init(4);

	puts("Adding 40 tasks to threadpool");
	int i;
	for (i=0; i<50; i++){
		thpool_add_work(thpool, task, (void*)(uintptr_t)i);
	};

	thpool_wait(thpool);
	//	puts("Killing threadpool");
	thpool_destroy(thpool);

	return 0;
}
