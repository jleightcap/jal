BIN  := main
SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)

INCLUDE = ./include
CFLAGS := -O0 -g -Wall -Wextra -pedantic -std=c99 -I$(INCLUDE) 
VALGRIND = valgrind.out

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS)

%o: %c $(wildcard $(INCLUDE)/*.h)
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	riscv64-linux-gnu-as -o $@ $^

%.bin: %.o
	riscv64-linux-gnu-ld -o $@ $^

# TEST=[testfile] make memtest
memtest: $(BIN)
	valgrind -q --track-origins=yes --leak-check=full --log-file=$(VALGRIND) ./$(BIN) $(TEST) /dev/null
	cat -n $(VALGRIND)

clean:
	rm -f src/*.o *.bin test/*.bin $(BIN) $(VALGRIND)
.PHONY: clean
