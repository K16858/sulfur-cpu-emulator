#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct state {
  uint16_t pc;
  uint16_t regs[8];
  uint16_t mem[65536];
  bool Z, N, C;
  bool running;
};

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
    printf("ALU");
    break;
  case 0b0001:
    printf("ADDI");
    break;
  case 0b0010:
    printf("SUBI");
    break;
  case 0b0011:
    printf("LOAD");
    break;
  case 0b0100:
    printf("STORE");
    break;
  case 0b0101:
    printf("BRANCH");
    break;
  case 0b0110:
    printf("JAL");
    break;
  case 0b0111:
    printf("RET");
    break;
  default:
    printf("undefined");
    return 1;
  }

  state->pc = next_pc;
  return 0;
}

int main(void) { struct state state; }
