#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>

int thread2()
{
	printf("Entered thread 2\n");
	exit(0);

	return 0;
}

int thread1()
{
	printf("Entered thread 1\n");
	uthread_create(thread2);
	printf("Created thread 2\n");

	// The only way for thread 2 to be entered is if an interrupt
	// causes it to switch from thread1 to thread2.
	while (1) {}

	return 0;
}

int main(void)
{
	uthread_start(0);
	uthread_join(uthread_create(thread1), NULL);
	uthread_stop();

	return 0;
}
