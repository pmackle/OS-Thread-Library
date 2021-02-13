#include <assert.h>
#include <limits.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

// All possible thread states.
#define READY 0
#define RUNNING 1
#define BLOCKED 2
#define ZOMBIE 3

// Queued threads to be dealt with.
static queue_t ready_processes;
static queue_t zombie_processes;
static queue_t blocked_processes;

static struct uthread* running;

// Initial conditions: preemption disabled, no instantiated threads.
static int preempt_required = 0;
static uthread_t num_processes = 0;

struct uthread {
	uthread_t tid;
	int tid_blocking;
	uthread_ctx_t* context;
	int state;
	void* stack;
	int already_joined;
};

static int manage_thread_library(struct uthread** myThread, int is_main) {
	
	*myThread = (struct uthread*)malloc(sizeof(struct uthread));
	// If failure, send error.
	if (!*myThread) {
		if (is_main) {
			// Free unused queues.
			queue_destroy(zombie_processes);
			queue_destroy(ready_processes);
		}

		return EXIT_FAILURE;
	}

	(*myThread)->context = (ucontext_t*)malloc(sizeof(ucontext_t));
	if (!(*myThread)->context) {
		if (is_main) {
			queue_destroy(ready_processes);
			queue_destroy(zombie_processes);
		}
		free(myThread);

		return EXIT_FAILURE;
	}

	// Adequately describe the newly created thread.
	(*myThread)->tid = num_processes;
	(*myThread)->state = is_main ? RUNNING : READY;
	(*myThread)->stack = NULL;
	(*myThread)->tid_blocking = -1;

	return EXIT_SUCCESS;
}

int uthread_start(int preempt)
{
	ready_processes = queue_create();
	// If failure, send error.
	if (!ready_processes) {
		return -1;
	}	

	zombie_processes = queue_create();
	// If failure, clean readies. Send error.
	if (!zombie_processes) {
		queue_destroy(ready_processes);

		return -1;
	}

	blocked_processes = queue_create();
	// If failure, clean readies. Send error.
	if (!blocked_processes) {
		queue_destroy(ready_processes);
		queue_destroy(zombie_processes);

		return -1;
	}
	
	if(manage_thread_library(&running, 1)) {
		return -1;
	}
	
	if (preempt) {
		preempt_required = preempt;
		preempt_start();
	}

	return 0;
}

int uthread_create(uthread_func_t func)
{
	preempt_disable();
	// If thread overflow, send error.
	if(num_processes >= USHRT_MAX){
		return -1;
	}

	num_processes++;
	struct uthread* myThread = NULL;
	if (manage_thread_library(&myThread, 0)) {
		num_processes--;

		return -1;
	}

	myThread->stack = uthread_ctx_alloc_stack();
	if (!myThread->stack) {
		free(myThread->context);
		free(myThread);
		
		return -1;
	}

	int context_init_error = uthread_ctx_init(myThread->context, myThread->stack, func);

	if (context_init_error) {
		uthread_ctx_destroy_stack(myThread->stack);
		free(myThread->context);
		free(myThread);

		return -1;
	}

	preempt_disable();

	if (queue_enqueue(ready_processes, (void*)myThread) == -1) {
		uthread_ctx_destroy_stack(myThread->stack);
		free(myThread->context);
		free(myThread);

		return -1;
	}
	
	preempt_enable();

	return myThread->tid;
}

static void uthread_destroy(struct uthread* myThread)
{
	// Free up all allocated blocks.
	if (myThread->stack) {
		uthread_ctx_destroy_stack(myThread->stack);
	}
	free(myThread->context);
	free(myThread);
}



void uthread_yield(void)
{
	preempt_disable();
	// Add running process back to the ready queue.
	if (running->state == RUNNING) {
		running->state = READY;
		queue_enqueue(ready_processes, (void*)running);
	} else if (running->state == BLOCKED) {
		queue_enqueue(blocked_processes, (void*)running);
	} 

	// Run the next ready process.
	struct uthread* current_process = running;
	queue_dequeue(ready_processes, (void**)&running);
	running->state = RUNNING;
	// Call context switch anytime we wish to switch processes.
	uthread_ctx_switch(current_process->context, running->context);
	preempt_enable();
}

uthread_t uthread_self(void)
{
	return running->tid;
}

static int find_tid(queue_t q, void *data, void *arg)
{
    struct uthread* a = (struct uthread*)data;
    uthread_t match = (uthread_t)(long)arg;
    (void)q; //unused

    if (a->tid == match)
        return 1;

    return 0;
}

int uthread_stop(void)
{
	// If not main thread, send error. 
	if(running->tid != 0){
		return -1;
	}

	// If processes not cleaned, send error.
	if(queue_length(ready_processes) != 0 && queue_length(zombie_processes) != 0){
		
		return -1;
	}

	while (queue_length(zombie_processes) > 0) {
		struct uthread* myThread;

		preempt_disable();
		queue_dequeue(zombie_processes, (void**)&myThread);
		// queue_delete(zombie_processes, &tid);
		preempt_enable();

		uthread_destroy(myThread);
	}	

	// Clean up.
	queue_destroy(ready_processes);
	queue_destroy(zombie_processes);
	queue_destroy(blocked_processes);
	uthread_destroy(running);

	if (preempt_required) {
		preempt_stop();
	}

	return 0;
}

void uthread_exit(int retval)
{
	(void)retval;

	preempt_disable();
	if (running->tid_blocking != -1) {
		struct uthread* blocked = NULL;
		queue_iterate(blocked_processes, find_tid, (void*)(size_t)running->tid_blocking, (void**)&blocked);

		blocked->state = READY;
		queue_delete(blocked_processes, blocked);
		queue_enqueue(ready_processes, (void*)blocked);
	}

	running->state = ZOMBIE;
	queue_enqueue(zombie_processes, (void*)running);
	
	uthread_yield();
}

int uthread_join(uthread_t tid, int *retval)
{
	(void)retval;

	struct uthread* tbj = NULL; // To be joined.

	queue_iterate(ready_processes, find_tid, (void*)(size_t)tid, (void**)&tbj);
	queue_iterate(zombie_processes, find_tid, (void*)(size_t)tid, (void**)&tbj);

	if (tid == 0 || running->tid == tid || tbj == NULL || tbj->already_joined) {
		return -1;
	}

	tbj->already_joined = 1;

	if (tbj->state == READY) {
		running->state = BLOCKED;
		tbj->tid_blocking = (int) running->tid;
		uthread_yield();

	} else if (tbj->state == ZOMBIE) {
	}

	return EXIT_SUCCESS;
}
