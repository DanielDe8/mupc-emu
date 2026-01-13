#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "operations.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

int8_t memory[0x10000]; // 64KB memory
int8_t registers[4]; // 4 general-purpose registers
int8_t stack[0x10000]; // Stack memory
uint16_t pc = 0; // Program counter
uint16_t sc = 0; // Stack pointer
bool carry_f = false; // Carry flag

bool jump_f = false; // Jump flag
uint16_t jump_address; // Jump address for operations

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "rb");

    if (!file) {
        fprintf(stderr, "Failed to open program file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    int ch;
    for (int i = 0; i < 0x10000; i++) {
        ch = fgetc(file);
        if (ch == EOF) break;

        uint8_t value = (uint8_t)ch;
        memory[i] = value;
    };

    fclose(file);

    while (true) {
        uint8_t instruction = memory[pc];

        uint8_t opcode = (instruction >> 4) & 0b00001111; // Get opcode
        uint8_t op1 = (instruction >> 2) & 0b00000011; // Get operand 1
        uint8_t op2 = instruction & 0b00000011; // Get operand 2

        // printf("Executing instruction at PC: %04X, Op1: %02X, Op2: %02X, Opcode: "BYTE_TO_BINARY_PATTERN"\n", pc, op1, op2, BYTE_TO_BINARY(opcode));

        operations[opcode](op1, op2); // Execute operation

        // printf("Registers: R0=%d, R1=%d, R2=%d, R3=%d\n", registers[0], registers[1], registers[2], registers[3]);
        // scanf("%*c"); // Wait for user input to continue

        if (jump_f) {
            pc = jump_address; // Update program counter if jump is set
            jump_f = false; // Reset jump flag
        } else {
            pc++; // Increment program counter
        }
    }

    return EXIT_SUCCESS;
}