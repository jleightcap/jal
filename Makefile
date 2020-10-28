ARCH    ?= _Z80
BIN     ?= jal
SRCS    ?= $(wildcard src/*.c)
OBJS    ?= $(SRCS:.c=.o)
INCLUDE ?= ./include
CFLAGS  ?= -D$(ARCH) \
	   -std=c99 -I$(INCLUDE) \
	   -Wall -Wextra -pedantic \
	   -Wno-unused-parameter -Wno-type-limits
# release flags
RFLAGS ?= -O3
# debug flags
DFLAGS ?= -O0 -g

VALGRIND ?= valgrind.out
VFLAGS   ?= --leak-check=full \
	    --show-leak-kinds=all \
	    --track-origins=yes \
	    --trace-children=yes \
	    --show-reachable=no \
	    --log-file=$(VALGRIND)

$(BIN): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c $(wildcard $(INCLUDE)/*.h)
	$(CC) $(CFLAGS) $(DFLAGS) -c -o $@ $<

%.o: %.s
	riscv64-linux-gnu-as -o $@ $^

%.bin: %.o
	riscv64-linux-gnu-ld -o $@ $^

# TEST=[testfile] make memtest
memtest: $(BIN)
	valgrind -q $(VFLAGS) ./$(BIN) $(TEST) /dev/null
	cat -n $(VALGRIND)

clean:
	rm -f src/*.o *.bin test/*.bin ./*.s $(BIN) $(VALGRIND)*

.PHONY: clean
