#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "operations.h"

int8_t memory[0x10000];
int8_t registers[4];
int8_t stack[0x10000];
uint16_t pc;
uint16_t sc;
bool carry_f;
bool jump_f;
uint16_t jump_address;

static int fail(const char *what, uint32_t address, int expected, int actual) {
    fprintf(stderr, "%s at 0x%04" PRIx32 ": expected 0x%04x, got 0x%04x\n",
            what, address, expected & 0xffff, actual & 0xffff);
    return 1;
}

static void set_address(uint16_t address) {
    registers[2] = (int8_t)(uint8_t)(address >> 8);
    registers[3] = (int8_t)(uint8_t)address;
}

static uint8_t expected_byte(uint16_t address) {
    return (uint8_t)(address ^ (address >> 8) ^ 0xa5u);
}

static int test_memory_all_addresses(void) {
    for (uint32_t wide = 0; wide <= UINT16_MAX; ++wide) {
        const uint16_t address = (uint16_t)wide;
        const uint8_t value = expected_byte(address);
        set_address(address);
        registers[1] = (int8_t)value;

        /* Dispatch the WRM and RDM instructions through the real opcode table. */
        operations[12](1, 1);
        if ((uint8_t)memory[address] != value)
            return fail("WRM wrote wrong cell/value", wide, value,
                        (uint8_t)memory[address]);

        registers[0] = 0;
        operations[12](0, 0);
        if ((uint8_t)registers[0] != value)
            return fail("RDM returned wrong value", wide, value,
                        (uint8_t)registers[0]);
    }
    return 0;
}

static int check_jump(uint32_t address, uint8_t opcode, uint8_t subop,
                      uint8_t lhs, uint8_t rhs, bool carry) {
    set_address((uint16_t)address);
    registers[0] = (int8_t)lhs;
    registers[1] = (int8_t)rhs;
    carry_f = carry;
    jump_f = false;
    jump_address = 0x1357;

    if (opcode == 8)
        operations[opcode](subop, 0);
    else
        operations[opcode](0, 1);
    if (!jump_f)
        return fail("taken jump did not set jump_f", address, 1, jump_f);
    if (jump_address != address)
        return fail("taken jump formed wrong address", address, address,
                    jump_address);
    return 0;
}

static int test_taken_jumps_all_addresses(void) {
    for (uint32_t address = 0; address <= UINT16_MAX; ++address) {
        /* SJM: JMP, JZ, JNZ, JC. */
        if (check_jump(address, 8, 0, 0, 0, false) ||
            check_jump(address, 8, 1, 0, 0, false) ||
            check_jump(address, 8, 2, 1, 0, false) ||
            check_jump(address, 8, 3, 0, 0, true) ||
            /* JE, JG, JL use signed 8-bit register comparisons. */
            check_jump(address, 9, 0, 0x80, 0x80, false) ||
            check_jump(address, 10, 0, 1, 0, false) ||
            check_jump(address, 11, 0, 0x80, 0x7f, false))
            return 1;
    }
    return 0;
}

static int check_untaken(uint8_t opcode, uint8_t subop, uint8_t lhs,
                         uint8_t rhs, bool carry) {
    const uint16_t address = 0x80ff;
    set_address(address);
    registers[0] = (int8_t)lhs;
    registers[1] = (int8_t)rhs;
    carry_f = carry;
    jump_f = false;
    jump_address = 0x2468;

    if (opcode == 8)
        operations[opcode](subop, 0);
    else
        operations[opcode](0, 1);
    if (jump_f)
        return fail("untaken branch set jump_f", address, 0, jump_f);
    if (jump_address != 0x2468)
        return fail("untaken branch changed jump_address", address, 0x2468,
                    jump_address);
    return 0;
}

static int test_untaken_jumps_and_signed_comparisons(void) {
    /* Check all SJM conditionals preserve the pending jump state when false. */
    if (check_untaken(8, 1, 1, 0, false) || /* JZ */
        check_untaken(8, 2, 0, 0, false) || /* JNZ */
        check_untaken(8, 3, 0, 0, false) || /* JC */
        check_untaken(9, 0, 0x80, 0x7f, false) || /* -128 != 127 */
        check_untaken(10, 0, 0x80, 0x7f, false) || /* -128 is not > 127 */
        check_untaken(11, 0, 0x7f, 0x80, false)) /* 127 is not < -128 */
        return 1;
    return 0;
}

static int test_rdm_into_address_registers(void) {
    static const uint16_t addresses[] = {0x0080, 0x7fff, 0x8000, 0x80ff,
                                         0xff00, 0xffff};
    for (size_t i = 0; i < sizeof(addresses) / sizeof(addresses[0]); ++i) {
        const uint16_t address = addresses[i];
        const uint8_t value = (uint8_t)(0x39u + i);
        memory[address] = (int8_t)value;

        set_address(address);
        operations[12](0, 2);
        if ((uint8_t)registers[2] != value)
            return fail("RDM into c used a changed address", address, value,
                        (uint8_t)registers[2]);

        set_address(address);
        operations[12](0, 3);
        if ((uint8_t)registers[3] != value)
            return fail("RDM into d used a changed address", address, value,
                        (uint8_t)registers[3]);
    }
    return 0;
}

int main(void) {
    if (test_memory_all_addresses() || test_taken_jumps_all_addresses() ||
        test_untaken_jumps_and_signed_comparisons() ||
        test_rdm_into_address_registers())
        return EXIT_FAILURE;

    puts("Address operation tests passed (all 65,536 addresses).");
    return EXIT_SUCCESS;
}
