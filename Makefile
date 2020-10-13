BIN     ?= jal
SRCS    ?= $(wildcard src/*.c)
OBJS    ?= $(SRCS:.c=.o)
INCLUDE ?= ./include
CFLAGS  ?= -O0 -g \
	   -Wall -Wextra -Wno-unused-parameter -pedantic \
	   -std=c99 \
	   -I$(INCLUDE) -DRISCV-64

VFLAGS ?= --leak-check=full \
	  --show-leak-kinds=all \
	  --track-origins=yes \
	  --error-exitcode=1 \
	  --trace-children=yes \
	  --show-reachable=no
VALGRIND ?= valgrind.out

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS)

%.o: %.c $(wildcard $(INCLUDE)/*.h)
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	riscv64-linux-gnu-as -o $@ $^

%.bin: %.o
	riscv64-linux-gnu-ld -o $@ $^

# TEST=[testfile] make memtest
memtest: $(BIN)
	valgrind -q $(VFLAGS) --log-file=$(VALGRIND) ./$(BIN) $(TEST) /dev/null
	cat -n $(VALGRIND)

clean:
	rm -f src/*.o *.bin test/*.bin ./*.s $(BIN) $(VALGRIND)*

.PHONY: clean
