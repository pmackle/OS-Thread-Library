#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"

struct queue_node {
    void* data;
    struct queue_node* next;
};

struct queue {
    struct queue_node* front;
    struct queue_node* back;
    int length;
};

queue_t queue_create(void)
{
	queue_t my_queue = (queue_t)malloc(sizeof(struct queue));
    if (my_queue == NULL) {
		return NULL;
    }

	my_queue->front = NULL;
	my_queue->back = NULL;
	my_queue->length = 0;

	return my_queue;
}

int queue_destroy(queue_t queue)
{
	if (queue == NULL || queue->length != 0) {
        return -1;
    }

    free(queue);
    queue = NULL;

    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
    if (queue == NULL || data == NULL) {
        return -1;
    }

    queue_node_t my_queue_node = (queue_node_t)malloc(sizeof(struct queue_node));
    if (my_queue_node == NULL) {
        return -1;
	}
	
	my_queue_node->data = data;
    my_queue_node->next = NULL;

    if (queue->length == 0) {
        queue->front = my_queue_node;
        queue->back = my_queue_node;    
    } else {
        queue->back->next = my_queue_node;
        queue->back = my_queue_node;
    }
    queue->length++;
	
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL || queue->length == 0) {
		return -1;
	}
	
	*data = queue->front->data;
	queue_node_t temp = queue->front;
	queue->front = queue->front->next;

	if (queue->front == NULL) {
		queue->back = NULL;
	}

	queue->length--;

	free(temp);

	return 0;
}

int queue_delete(queue_t queue, void *data)
{
    if (queue == NULL || data == NULL || queue->length == 0) {
        return -1;
    }

	queue_node_t current_queue_node = queue->front;
	queue_node_t previous_queue_node;

	// Iterate through until node before last node
	while (current_queue_node != NULL){
		if(current_queue_node->data == data){
			queue->length--;
			break;
		}
		previous_queue_node = current_queue_node;
		current_queue_node = current_queue_node->next;
	}
	
	if (current_queue_node == queue->front && queue->front == queue->back){
		free(current_queue_node);
		queue->front = NULL;
		queue->back = NULL;
		return 0;
	}
	else if (current_queue_node == queue->back){
		free(queue->back);
		queue->back = previous_queue_node;
		queue->back = previous_queue_node;
		queue->back->next = NULL;
		return 0;
	}
	else if (current_queue_node == queue->front){
		queue_node_t new_front_node = queue->front->next;
		free(queue->front);
		queue->front = new_front_node;
		return 0;
	}
	else if (current_queue_node != NULL){
		previous_queue_node->next = current_queue_node->next;
		free(current_queue_node);
		return 0;
	}
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	if (queue == NULL || func == NULL) {
		return -1;
	}

	queue_node_t current_node = queue->front;
	while (current_node != NULL) {
		if (!((*func)(queue, current_node->data, arg))) {
			current_node = current_node->next;
		} else {
			*data = current_node->data;
			return 0;
		}
	}

	return 0;
}

int queue_length(queue_t queue)
{
	if (queue == NULL) {
        return -1;
    }

    return queue->length;
}
