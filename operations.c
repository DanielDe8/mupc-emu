#include "operations.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"

static uint16_t address_word(void) {
    /* c and d hold signed data bytes, but addresses use their unsigned values. */
    return ((uint16_t)(uint8_t)registers[2] << 8) | (uint8_t)registers[3];
}

void mov_op(uint8_t op1, uint8_t op2) {
    registers[op1] = registers[op2];
}
void add_op(uint8_t op1, uint8_t op2) {
    registers[op1] = registers[op1] + registers[op2];
    carry_f = (registers[op1] < registers[op2]); // Set carry flag if overflow occurs
}
void sub_op(uint8_t op1, uint8_t op2) {
    registers[op1] = registers[op1] - registers[op2];
    carry_f = (registers[op1] > registers[op2]); // Set carry flag if underflow occurs
}
void and_op(uint8_t op1, uint8_t op2) {
    registers[op1] = registers[op1] & registers[op2];
    carry_f = 0;
}
void or_op(uint8_t op1, uint8_t op2) {
    registers[op1] = registers[op1] | registers[op2];
    carry_f = 0;
}
void xor_op(uint8_t op1, uint8_t op2) {
    registers[op1] = registers[op1] ^ registers[op2];
    carry_f = 0;
}
void shr_op(uint8_t op1, uint8_t op2) {
    registers[op1] = registers[op1] >> registers[op2];
    carry_f = 0;
}
void idc_op(uint8_t op1, uint8_t op2) { // Increment/decrement
    if (op1 == 0) { // Increment
        registers[op2]++;
        carry_f = (registers[op2] == 0); // Set carry flag if overflow occurs
    } else if (op1 == 1) { // Decrement
        registers[op2]--;
        carry_f = (registers[op2] < 0); // Set carry flag if underflow occurs
    } else if (op1 == 2) { // Add carry
        registers[op2] += carry_f ? 1 : 0;
        carry_f = (registers[op2] < memory[pc]); // Set carry flag if overflow occurs
    } else if (op1 == 3) { // Subtract carry
        registers[op2] -= carry_f ? 1 : 0;
        carry_f = (registers[op2] < 1); // Set carry flag if underflow occurs
    }
}
void sjm_op(uint8_t op1, uint8_t op2) { // Simple jump
    if (op1 == 0) {
        jump_f = 1;
        jump_address = address_word();
    } else if (op1 == 1) { // Jump if zero
        if (registers[op2] == 0) {
            jump_f = 1;
            jump_address = address_word();
        }
    } else if (op1 == 2) { // Jump if not zero
        if (registers[op2] != 0) {
            jump_f = 1;
            jump_address = address_word();
        }
    } else if (op1 == 3) { // Jump if carry
        if (carry_f) {
            jump_f = 1;
            jump_address = address_word();
        }
    }
    
}
void je_op(uint8_t op1, uint8_t op2) {
    if (registers[op1] == registers[op2]) {
        jump_f = 1;
        jump_address = address_word();
    }
}
void jg_op(uint8_t op1, uint8_t op2) {
    if (registers[op1] > registers[op2]) {
        jump_f = 1;
        jump_address = address_word();
    }
}
void jl_op(uint8_t op1, uint8_t op2) {
    if (registers[op1] < registers[op2]) {
        jump_f = 1;
        jump_address = address_word();
    }
}
void mst_op(uint8_t op1, uint8_t op2) { // 2 different memory operations + 2 stack operations
    if (op1 == 0) { // Read from memory
        registers[op2] = memory[address_word()];
        // printf("m[%d] = %0d\n", ((uint8_t) (registers[2]) << 8) | (uint8_t) (registers[3]), memory[((uint8_t) (registers[2]) << 8) | (uint8_t) (registers[3])]);
    } else if (op1 == 1) { // Write to memory
        memory[address_word()] = registers[op2];
    } else if (op1 == 2) { // Pop from stack
        registers[op2] = stack[--sc];
    } else if (op1 == 3) { // Push to stack
        stack[sc++] = registers[op2];
    }
}
void io_op(uint8_t op1, uint8_t op2) { // I/O operations
    uint8_t channel = memory[++pc];
    
    if (op1 == 0) {
        scanf("%s", &registers[op2]);
    } else if (op1 == 1) {
        printf("%c", registers[op2]);
    }
}
void msc_op(uint8_t op1, uint8_t op2) { // Miscellaneous operations
    if (op1 == 0) { // Load one
        registers[op2] = 1;
    } else if (op1 == 1) { // Load random
        registers[op2] = rand() % 256;
    } else if (op1 == 2) { // Shut down
        exit(EXIT_SUCCESS);
    } else if (op1 == 3) { // Reboot
        // TODO
    }
}
void imm_op(uint8_t op1, uint8_t op2) {
    if (op1 == 0) { // Load immediate
        registers[op2] = memory[++pc];
    } else if (op1 == 1) { // Add immediate
        registers[op2] += memory[++pc];
    } else if (op1 == 2) {

    } else if (op1 == 3) {

    }
}

void (*operations[16])(uint8_t op1, uint8_t op2) = {
    mov_op,  // 0
    add_op,  // 1
    sub_op,  // 2
    and_op,  // 3
    or_op,   // 4
    xor_op,  // 5
    shr_op,  // 6
    idc_op,  // 7 sub
    sjm_op,  // 8 sub
    je_op,   // 9
    jg_op,   // 10
    jl_op,   // 11
    mst_op,  // 12 sub
    io_op,   // 13 sub
    msc_op,  // 14 sub
    imm_op   // 15 sub
};
