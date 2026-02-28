#ifndef CPU_H
#define CPU_H

#include "emulator.h"

void init_state(struct state *state);
uint16_t sign_extend(uint16_t imm, int bit_count);

uint16_t alu(uint16_t op, uint16_t a, uint16_t b);

void decode(uint16_t instr, struct instruction *decode_result);
int execute(struct state *state, struct instruction instr);

int step(struct state *state);

#endif
