#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queue {
	/* TODO */
};

queue_t queue_create(void)
{
	/* TODO */
	return NULL;
}

int queue_destroy(queue_t queue)
{
	/* TODO */
	return -1;
}

int queue_enqueue(queue_t queue, void *data)
{
	/* TODO */
	return -1;
}

int queue_dequeue(queue_t queue, void **data)
{
	/* TODO */
	return -1;
}

int queue_delete(queue_t queue, void *data)
{
	/* TODO */
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	/* TODO */
	return -1;
}

int queue_length(queue_t queue)
{
	/* TODO */
	return -1;
}

