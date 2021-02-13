#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	queue_t queue;
	int data = 3, *ptr;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	queue = queue_create();
	queue_enqueue(queue, &data);
	queue_dequeue(queue, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

void test_queue_length(){
	int set[3] = {0, 1, 2};
	queue_t queue = queue_create();

	fprintf(stderr, "*** TEST queue length ***\n");

	for (int i = 0; i < 3; i++) {
		queue_enqueue(queue, &set[i]);	
	}

	TEST_ASSERT(queue_length(queue) == 3);
}

void test_queue_null_length(){
	queue_t queue = NULL;
	fprintf(stderr, "*** TEST null queue length ***\n");

	int error_value = queue_length(queue);
	TEST_ASSERT(error_value == -1);
}

void test_delete(void) 
{
	int set[3] = {0, 1, 2};
	queue_t queue = queue_create();

	fprintf(stderr, "*** TEST delete queue ***\n");

	for (int i = 0; i < 3; i++) {
		queue_enqueue(queue, &set[i]);	
	}

	queue_delete(queue, &set[1]);
	TEST_ASSERT(queue_length(queue) == 2);
}

void test_delete_not_found(void)
{
	int set[3] = {0, 1, 2};
	int doesnt_exist = 100;
	queue_t queue = queue_create();

	fprintf(stderr, "*** TEST delete not found value ***\n");

	for (int i = 0; i < 3; i++) {
		queue_enqueue(queue, &set[i]);	
	}

	int error_value = queue_delete(queue, &doesnt_exist);
	TEST_ASSERT(error_value == -1);
}

void test_queue_destroy()
{
	int set[3] = {0, 1, 2};
	int *ptr;

	queue_t queue = queue_create();

	fprintf(stderr, "*** TEST destroy queue ***\n");

	for (int i = 0; i < 3; i++) {
		queue_enqueue(queue, &set[i]);	
	}

	for (int i = 0; i < 3; i++) {
		queue_dequeue(queue, (void**)&ptr);	
	}

	int error_value = queue_destroy(queue);
	TEST_ASSERT(error_value == EXIT_SUCCESS);
}

void test_queue_destroy_not_empty(){
	int set[3] = {0, 1, 2};

	queue_t queue = queue_create();

	fprintf(stderr, "*** TEST destroy not empty queue ***\n");

	for (int i = 0; i < 3; i++) {
		queue_enqueue(queue, &set[i]);	
	}

	int error_value = queue_destroy(queue);
	TEST_ASSERT(error_value == -1);
}

void test_dequeue_size_zero() {
	int *ptr;

	fprintf(stderr, "*** TEST dequeue empty queue***\n");

	queue_t queue = queue_create();

	TEST_ASSERT(queue_dequeue(queue, (void**)&ptr) == -1);
}

void test_dequeue_string_array() {
	char** ptr;
	char* set[] = {"zero", "one", "two"};
	queue_t queue = queue_create();

	fprintf(stderr, "*** TEST dequeue string queue***\n");
	
	for (int i = 0; i < 3; i++) {
		queue_enqueue(queue, &set[i]);	
	}

	queue_dequeue(queue, (void**)&ptr);

	TEST_ASSERT(!strcmp(*ptr, "zero"));

}

// Taken from project prompt.

/* Callback function that increments integer items by a certain value (or delete
 * item if item is value 42) */
int inc_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int inc = (int)(long)arg;
	(void)q;

    *a += inc;

    return 0;
}

/* Callback function that finds a certain item according to its value */
int find_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int match = (int)(long)arg;
    (void)q; //unused

    if (*a == match)
        return 1;

    return 0;
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 42, 6, 7, 8, 9};
    size_t i;
	int *ptr;

	fprintf(stderr, "*** TEST iterator ***\n");
    /* Initialize the queue and enqueue items */
    q = queue_create();

    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    /* Add value '1' to every item of the queue, delete item '42' */
    queue_iterate(q, inc_item, (void*)1, NULL);

	TEST_ASSERT(queue_length(q) == 10 && data[0] ==  2);

    /* Find and get the item which is equal to value '5' */
    ptr = NULL;     // result pointer *must* be reset first
    queue_iterate(q, find_item, (void*)5, (void**)&ptr);

	TEST_ASSERT(ptr != NULL && *ptr == 5 && ptr == &data[3]);

}

int main(void)
{
	test_create();
	test_queue_simple();
	test_queue_length();
	test_queue_null_length();
	test_delete();
	test_delete_not_found();
	test_queue_destroy();
	test_queue_destroy_not_empty();
	test_iterator();
	test_dequeue_size_zero();
	test_dequeue_string_array();
	
	return 0;
}