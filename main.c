#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct state {
  uint16_t pc;
  uint16_t regs[8];
  uint16_t mem[65536];
  bool running;
};

uint16_t alu(uint16_t op, uint16_t a, uint16_t b) {
  uint32_t a32 = (uint32_t)a;
  uint32_t b32 = (uint32_t)b;
  uint32_t result = 0;
  uint16_t shift = b & 0xF;

  switch (op) {
  case 0b000:
    // ADD
    result = a32 + b32;
    return (uint16_t)result;
  case 0b001:
    // SUB
    result = a32 - b32;
    return (uint16_t)result;
  case 0b010:
    // SLT
    result = ((int16_t)a < (int16_t)b) ? 1 : 0;
    return (uint16_t)result;
  case 0b011:
    // AND
    return a & b;
  case 0b100:
    // OR
    return a | b;
  case 0b101:
    // XOR
    return a ^ b;
  case 0b110:
    // SHL
    if (shift == 0) {
      return a;
    }

    return (uint16_t)(a << shift);
  case 0b111:
    // SHR
    if (shift == 0) {
      return a;
    }

    return (uint16_t)(a >> shift);
  default:
    return 0;
  }
}

void init_state(struct state *state) { memset(state, 0, sizeof(struct state)); }

int step(struct state *state) {
  uint16_t instr = state->mem[state->pc];
  uint16_t next_pc = state->pc + 1;

  // 0000(op) 000(rd) 000(rs1) 000(rs2) 000(func)
  uint16_t opcode = instr >> 12;
  uint16_t func = instr & 0b0000000000000111;
  uint16_t rs2 = (instr & 0b0000000000111000) >> 3;
  uint16_t rs1 = (instr & 0b0000000111000000) >> 6;
  uint16_t rd = (instr & 0b0000111000000000) >> 9;

  // 0000(op) 000(rd) 000(rs1) 000000(imm)
  uint16_t imm = instr & 0b0000000000111111;

  // sign extension
  uint16_t simm;
  // bit5 == 1 (negative)
  if (imm & 0b0000000000100000) {
    simm = imm | 0b1111111111100000;
  } else {
    simm = imm;
  }

  switch (opcode) {
  case 0b0000:
    printf("ALU\n");
    state->regs[rd] = alu(func, state->regs[rs1], state->regs[rs2]);
    break;
  case 0b0001:
    printf("ADDI\n");
    state->regs[rd] = state->regs[rs1] + simm;
    break;
  case 0b0010:
    printf("SUBI\n");
    state->regs[rd] = state->regs[rs1] - simm;
    break;
  case 0b0011:
    printf("LOAD\n");
    uint16_t addr = state->regs[rs1] + simm;
    state->regs[rd] = state->mem[addr];
    break;
  case 0b0100:
    printf("STORE\n");
    if (addr < 0x2000) {
      printf("Write to ROM!\n");
      return 1;
    }
    uint16_t addr = state->regs[rs1] + simm;
    state->mem[addr] = state->regs[rd];
    break;
  case 0b0101:
    printf("BEQ\n");
    if (state->regs[rd] == state->regs[rs1]) {
      next_pc += (int16_t)simm;
    }
    break;
  case 0b0110:
    printf("BNE\n");
    if (state->regs[rd] != state->regs[rs1]) {
      next_pc += (int16_t)simm;
    }
    break;
  case 0b0111:
    printf("JAL\n");
    break;
  case 0b1000:
    printf("RET\n");
    break;
  default:
    printf("undefined\n");
    return 1;
  }

  state->pc = next_pc;
  return 0;
}

int main(void) {
  static struct state state;
  init_state(&state);
  state.mem[0] = 0b0000000000000111;
  state.mem[1] = 0b0100000000000000;

  step(&state);
  step(&state);
}
