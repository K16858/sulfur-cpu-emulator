#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct state {
  uint16_t pc;
  uint16_t regs[8];
  uint16_t mem[65536];
  bool Z, N, C;
  bool running;
};

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

  switch (opcode) {
  case 0b0000:
    printf("ALU\n");
    break;
  case 0b0001:
    printf("ADDI\n");
    break;
  case 0b0010:
    printf("SUBI\n");
    break;
  case 0b0011:
    printf("LOAD\n");
    break;
  case 0b0100:
    printf("STORE\n");
    break;
  case 0b0101:
    printf("BRANCH\n");
    break;
  case 0b0110:
    printf("JAL\n");
    break;
  case 0b0111:
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
