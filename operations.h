#ifndef __OPERATIONS_H
#define __OPERATIONS_H

#include <stdint.h>
#include <stdlib.h>

extern void(*operations[16])(uint8_t op1, uint8_t op2);

#endif // __OPERATIONS_H