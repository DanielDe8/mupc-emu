.PHONY: all clean run install

OBJS = main.o operations.o
HEADER = main.h operations.h
CC = gcc
OUT = mupc-emu
EMU_ARGS = ../mupc-asm/out.mupc
INSTALL_DIR = /usr/local/bin

all: clean $(OUT)

$(OUT): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c $(HEADER)
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUT)

run: all
	./$(OUT) $(EMU_ARGS)

install: all
	sudo rm $(INSTALL_DIR)/$(OUT)
	sudo cp $(OUT) $(INSTALL_DIR)/