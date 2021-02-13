lib := libuthread.a
objs := queue.o context.o uthread.o preempt.o

CC := gcc
CFLAGS := -Wall -Wextra -Werror -MMD

ifneq ($(V),1)
Q = @
endif

all: $(lib)

deps := $(patsubst %.o, %.d, $(objs))
-include $(deps)

$(lib): $(objs)
	@echo "CC	$@"
	ar rcs $@ $^

%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "CLEAN"
	$(Q)rm -f $(lib) $(objs) $(deps) *.d