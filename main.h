#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

extern int8_t memory[0x10000]; // 64KB memory
extern int8_t registers[4]; // 4 general-purpose registers
extern int8_t stack[0x10000]; // Stack memory
extern uint16_t pc; // Program counter
extern uint16_t sc; // Stack pointer
extern bool carry_f; // Carry flag

extern bool jump_f; // Jump flag
extern uint16_t jump_address; // Jump address for operations

#endif // __MAIN_H