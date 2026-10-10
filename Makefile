OBJS = main.o operations.o
HEADER = main.h operations.h
CC = gcc
OUT = mupc-emu
EMU_ARGS = ../mupc-asm/out.mupc

all: $(OUT)

$(OUT): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c $(HEADER)
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUT) $(TEST_OUT)

run: $(OUT)
	./$(OUT) $(EMU_ARGS)
