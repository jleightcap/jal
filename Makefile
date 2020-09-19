BIN  := main
SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)

CFLAGS := -O0 -g -Wall -Wextra -pedantic -std=c99 -Iinclude/

VALGRIND = valgrind.out

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS)

%o: %c $(wildcard include/*.h)
	$(CC) $(CFLAGS) -c -o $@ $<

%: %.o
	riscv64-linux-gnu-ld -o $@ $^

%.o: %.s
	riscv64-linux-gnu-as -o $@ $^

# TEST=[testfile] make memtest
memtest: $(BIN)
	valgrind -q --track-origins=yes --leak-check=full --log-file=$(VALGRIND) ./$(BIN) $(TEST)
	cat -n $(VALGRIND)

clean:
	rm -f *.o $(BIN)
.PHONY: clean
