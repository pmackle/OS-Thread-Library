
# ECS 150 Project #2 - User-Level Thread Library
## Table of Contents

-  [Introduction](#introduction)

-  [Overview](#overview)

-  [High-Level Process](#high-level-process)

-  [Queue API](#queue-API)

-  [Uthread API](#uthread-api)

-  [uthread_join()](#uthread_join())

-  [Preemption](#preemption)

-  [Sources](#sources)

## Introduction
The User-Level Thread Library project is an assignment designed to create an
intimate understanding of how threads and process scheduling works in an
operating system. The project uses *queues* to keep track of the threads and has
the option for *preemption* to schedule the threads in a round-robin fashion. 

## Overview

### High-Level Process
The implementation of the program is as follows:
1. Create a Queue API with a linked list data structure to allow for most
   operations to be O(1) complexity.
2. Once a thread has been created, begin the process of initializing the `ready`
   and `zombie` queues. As each thread is created, allocate the information
   needed for each thread in a struct.
3. Handle each of the threads as they come along, `yield()` as necessary to the
   next thread in the queue, and schedule them in split, fixed intervals if
   `preempt` is enabled.
4. Free all queues and threads with `uthread_stop()` once the threads have been
   joined and exited.
4. Use unit tests to make sure that each API is behaving as it should under
   normal use and certain edge cases.

### Queue API

#### Implementation
For our Queue API, we used a linked list data structure for its ease of
implementation and ability to have constant computational complexity for the
processes we needed. Arrays have constant complexity while indexing; however,
arrays have linear computational complexity for inserting and deleting, which is
a task that we constantly run in a user thread library. We only care about
whatever is next in the queue and the ability to append to it in O(1) time. 

Because we want the queue to operate with any type of data we throw at it, we
use the C++ equivalent of templates for C by using type `void`. This allows us
much more flexibility.

#### Testing
We tested each of the features that were used in the queue, as well as what
happens to the queue under certain edge cases where errors should be raised. 

We expect the queue in normal operation to be able to `create` the queue,
`enqueue` data, `dequeue` it, `iterate`, `destroy` data, and signal the user
with an `-EXIT_FAILURE` macro if unsuccessful (-1). We test the edge cases such
as trying to delete an item that does not exist, destroy a queue that is empty,
and several more. Testing these reassure us that all the behavior we expect from
the queue is defined and gives us the information we need if it succeeds or
fails.


### Uthread API

#### Implementation
The Uthread API is a way to create, schedule, and manage threads. In our
implementation, we define the possible states of our threads through macros
`READY`, `RUNNING`, `BLOCKED`, `ZOMBIE`, as they are values that will never get
changed, only assigned to other data. We also created two static queues to keep
track of the ready threads as well as the zombie threads. The global queues
themselves hold `uthread` structs, which have properties that keep track of each
individual thread, like its TID and context. By using these structs, we are
easily able to keep track of every queue, which thread is next in each queue,
and an easy way to access the data of the currently running queue as well as the
next in the queue. 

We also have two other static variables, for when preempt is enabled and the
number of processes that have been started. Using global variables and structs
allows us to manipulate them in multiple functions without having to change the
parameters for the functions in the UThread library. 

We implement some helper functions such as `find_tid`, `uthread_destroy`, and
`manage_thread_library` to help reduce redundancy in our code and to improve
readability.

#### Testing
To test uThread, we used the professors original tests. The original test simply
creates one thread to see if it prints `"Hello World!` to the console.

### uthread_join()

#### Implementation
For `uthread_join()`, we removed the infinite loop we were previously using and
instead set up the logic so that when a thread tries to join another active
thread, it must become blocked until the other thread also dies. We kept track
of an `already_joined` property in the struct to determine whether or not a
thread is going to become blocked or not. 

#### Testing
To test `uthread_join()` we create three different threads and call join from
the main, thread1, and thread2 functions. The threads yield to one another and
make sure that it consistently prints from thread3->1.

### Preemption

#### Implementation
For preemption, we have several static structs for the timer and alarms to keep
track of when to switch threads. We create a `timer_handler` function that
yields whatever function is in process whenever a `SIGVTALRM` goes off every
.001 seconds. The other functions are responsible for enabling and disabling the
signals so that they don't interrupt critical sections in the code. The start
and stop functions use our structs to initialize what the time interval will be
and how those alarms will be raised.

#### Testing
This test is actually fairly straightforward, and a great example of why
`preempt` oftentimes is needed. In summary, `test_preempt.x` consists of two
threads that print when they have been created and entered. Thread 1 creates
thread 2, and with `preempt` disabled, there is no way for thread 1 to switch to
thread 2 until thread 1 is finished. So, thread 1 enters an infinite while loop
and never exits, thereby preventing thread 2 from ever being created.

However, if and only if prempt is enabled, then every .001 seconds it will
switch from thread 1 to thread 2 after thread 2 has been created. So rather than
be stuck in thread 1's loop forever, it will switch over to thread 2 once the
alarm is raised after the time interval, thereby allowing the thread 2 to print
and exit.

## Sources
Cited in the code whenever something is referenced.