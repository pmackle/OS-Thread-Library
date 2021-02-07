# Target library
lib := libuthread.a
targets := lib
objs := queue.o
CC := gcc
CFLAGS := -MMD #-Wall -Wextra -Werror
CFLAGS += -g
LDFLAGS := -l

## If V=1, then show us verbose version of compiling
ifneq ($(V),1)
Q = @
endif



all: $(lib)

## Keeps track of dependencies (i.e .h files) for compiling
deps := $(patsubst %.o, %.d, $(objs))
-include $(deps)

## TODO
## Reminder
## $@ = Name of target
## $< = Name of first prerequisite
## $^ = Names of all prerequisites
## %.o: %.c how to generate any file <file>.o from another <file>.c
libuthread.a: $(objs)
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "CLEAN"
	$(Q)rm -f $(targets) $(objs) $(deps)