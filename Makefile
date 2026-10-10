OBJS = main.o operations.o
HEADER = main.h operations.h
CC = gcc
OUT = mupc-emu
TEST_OUT = tests/address_test.out
EMU_ARGS = ../mupc-asm/out.mupc

.PHONY: all clean run test

all: $(OUT)

$(OUT): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c $(HEADER)
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUT) $(TEST_OUT)

run: $(OUT)
	./$(OUT) $(EMU_ARGS)

$(TEST_OUT): tests/address_test.c operations.c $(HEADER)
	$(CC) $(CPPFLAGS) $(CFLAGS) -I. tests/address_test.c operations.c $(LDFLAGS) $(LDLIBS) -o $@

test: $(TEST_OUT)
	$(TEST_OUT)
