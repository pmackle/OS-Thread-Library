#include <stdint.h>
#include <stdlib.h>
#include <string.h>
// FIXME: Remove.
#include <stdio.h>

#include "queue.h"

// FIXME: Add comments to everything.
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
    queue_node_t my_queue_node = (queue_node_t)malloc(sizeof(struct queue_node));
    if (my_queue_node == NULL) {
		return NULL;
    }

	my_queue_node->data = NULL;
    my_queue_node->next = NULL;

    queue_t my_queue = (queue_t)malloc(sizeof(struct queue));
    if (my_queue == NULL) {
		return NULL;
    }

	my_queue->front = my_queue_node;
	my_queue->back = my_queue_node;
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
		printf("Entered back\n");
		free(queue->back);
		queue->back = previous_queue_node;
		queue->back = previous_queue_node;
		queue->back->next = NULL;
		return 0;
	}
	else if (current_queue_node == queue->front){
		printf("Entered front\n");
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

// FIXME: Test this.
int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	if (queue == NULL || func == NULL) {
		return -1;
	}

	queue_node_t current_node = queue->front;
	while (current_node != NULL) {
		if (!((*func)(queue, current_node->data, arg))) {
			continue;
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

// FIXME: Remove.
void queue_print(queue_t queue){
	printf("Queue length: %d\n", queue->length);
	printf("Queue contents: [");
	queue_node_t current_node = queue->front;
	while (current_node != NULL) {
		printf("%d, ", (int)current_node->data);
		current_node = current_node->next;
	}
	if (queue->length != 0) {
		printf("\b\b]\n");
	} else {
		printf("]\n");
	}

}

// FIXME: Remove.
int main(){
	int data;
	int* ptr = &data;

	queue_t queue = queue_create();
	queue_enqueue(queue, 1);
	queue_print(queue);
	queue_enqueue(queue, 2);
	queue_print(queue);
	queue_enqueue(queue, 3);
	queue_print(queue);

	queue_dequeue(queue, &ptr);
	queue_print(queue);
	queue_dequeue(queue, &ptr);
	queue_print(queue);
	queue_dequeue(queue, &ptr);
	queue_print(queue);
	
	queue_enqueue(queue, 1);
	queue_print(queue);
	queue_enqueue(queue, 2);
	queue_print(queue);
	queue_enqueue(queue, 3);
	queue_print(queue);
	queue_enqueue(queue, 4);
	queue_print(queue);

	queue_delete(queue, 2);
	queue_print(queue);
	queue_delete(queue, 1);
	queue_print(queue);
	queue_delete(queue, 4);
	queue_print(queue);
	queue_delete(queue, 3);
	queue_print(queue);

	queue_enqueue(queue, 1);
	queue_print(queue);

	int return_val = queue_delete(queue, 2);
	printf("Delete retd %d!\n", return_val);
	queue_print(queue);

	queue_dequeue(queue, &ptr);
	queue_print(queue);

	return_val = queue_destroy(queue);
	printf("Destroy retd %d!\n", return_val);

	return 0;
}
