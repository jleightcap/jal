ARCH    ?= RISCV_64
BIN     ?= jal
SRCS    ?= $(wildcard src/*.c)
OBJS    ?= $(SRCS:.c=.o)
INCLUDE ?= ./include
CFLAGS  ?= -D$(ARCH) \
	   -O0 -g -Wall -Wextra -Wno-unused-parameter -pedantic \
	   -std=c99 -I$(INCLUDE)

VALGRIND ?= valgrind.out
VFLAGS ?= --leak-check=full \
	  --show-leak-kinds=all \
	  --track-origins=yes \
	  --trace-children=yes \
	  --show-reachable=no \
	  --log-file=$(VALGRIND)

$(BIN): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c $(wildcard $(INCLUDE)/*.h)
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	riscv64-linux-gnu-as -o $@ $^

%.bin: %.o
	riscv64-linux-gnu-ld -o $@ $^

# TEST=[testfile] make memtest
memtest: $(BIN)
	valgrind -q $(VFLAGS)  ./$(BIN) $(TEST) /dev/null
	cat -n $(VALGRIND)

clean:
	rm -f src/*.o *.bin test/*.bin ./*.s $(BIN) $(VALGRIND)*

.PHONY: clean
