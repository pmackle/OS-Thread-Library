#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define INITIALIZE_VALUE 0

// Ref - https://man7.org/linux/man-pages/man2/sigaction.2.html
static sigset_t block_alarm;
static sigset_t old_set;
static struct itimerval timer; // when a timer should expire
static struct itimerval old_timer;
static struct sigaction act;
static struct sigaction old_act;


// Timer interrupt handler
static void timer_handler(int signum)
{
	if (signum == SIGVTALRM)
		uthread_yield();
}

// Ref - https://www.gnu.org/software/libc/manual/html_node/Setting-an-Alarm.html
// https://stackoverflow.com/questions/9407556/why-are-both-tv-sec-and-tv-usec-significant-in-determining-the-duration-of-a-tim

// We keep the old alarm to restore when multithreading is done
void preempt_start(void)
{
	// Initializing.
	act.sa_handler = timer_handler; // "The action to be associated with signum"
	sigemptyset(&act.sa_mask); // initialize
	act.sa_flags = INITIALIZE_VALUE;
	timer.it_interval.tv_sec = INITIALIZE_VALUE; // Will always be below 1 full second
	timer.it_value.tv_sec = INITIALIZE_VALUE;

	sigemptyset(&block_alarm);
	sigaddset(&block_alarm, SIGVTALRM); // The alarm signal responsible for blocking.

	// To set it off 100 times a second
	timer.it_interval.tv_usec = (long int) (1.0 / HZ * 1000000);
	timer.it_value.tv_usec = (long int) (1.0 / HZ * 1000000);

	sigprocmask(SIG_SETMASK, NULL, &old_set); // Store main thread signal mask
	setitimer(ITIMER_VIRTUAL, &timer, &old_timer);
	sigaction(SIGVTALRM, &act, &old_act);  // Signal handler initialization
}

// Restore all the previous config and values from before multithreading
void preempt_stop(void)
{
	setitimer(SIGVTALRM, &old_timer, NULL);
	sigaction(SIGVTALRM, &old_act, NULL);
	sigprocmask(SIG_SETMASK, &old_set, NULL);
}

// Unblocking signals
void preempt_enable(void)
{
	sigprocmask(SIG_UNBLOCK, &block_alarm, NULL);
}

// Blocking signals
void preempt_disable(void)
{
	sigprocmask(SIG_BLOCK, &block_alarm, NULL);
}
