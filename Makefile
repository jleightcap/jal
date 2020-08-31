BIN  := main
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

CFLAGS := -O0 -g -Wall -Wextra -pedantic -std=c99

$(BIN): $(OBJS)
	$(CC) -o $@ $(OBJS)

%o: %c $(wildcard *.h)
	$(CC) $(CFLAGS) -c -o $@ $<
%: %.o
	riscv64-linux-gnu-ld -o $@ $^

%.o: %.s
	riscv64-linux-gnu-as -o $@ $^

clean:
	rm -f *.o $(BIN)
