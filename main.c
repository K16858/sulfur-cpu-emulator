#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

struct state {
  uint16_t pc;
  uint16_t regs[8];
  uint16_t mem[65536];
  bool running;
};

struct instruction {
  uint16_t opcode;
  uint16_t rd;
  uint16_t rs1;
  uint16_t rs2;
  uint16_t func;
  int16_t imm;
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

void init_state(struct state *state) {
  memset(state, 0, sizeof(struct state));
  state->running = true;
}

uint16_t sign_extend(uint16_t imm, int bit_count) {
  int16_t s_imm = (int16_t)(imm << (16 - bit_count));
  return (uint16_t)(s_imm >> (16 - bit_count));
}

void decode(uint16_t instr, struct instruction *decode_result) {
  // 0000(op) 000(rd) 000(rs1) 000(rs2) 000(func)
  decode_result->opcode = instr >> 12;
  decode_result->func = instr & 0b0000000000000111;
  decode_result->rs2 = (instr & 0b0000000000111000) >> 3;
  decode_result->rs1 = (instr & 0b0000000111000000) >> 6;
  decode_result->rd = (instr & 0b0000111000000000) >> 9;

  switch (decode_result->opcode) {
  case 0b0011: // LOAD
  case 0b0100: // STORE
    // 0000(op) 000(rd) 000(rs) 000000(imm)
    decode_result->imm = (uint16_t)sign_extend(instr & 0b0000000000111111, 6);
    break;
  case 0b0001: // ADDI
  case 0b0010: // SUBI
  case 0b0101: // BEQZ
  case 0b0110: // BNEZ
  case 0b1001: // LUI
    // 0000(op) 000(rd) 000000000(imm)
    decode_result->imm = (uint16_t)sign_extend(instr & 0b0000000111111111, 9);
    break;
  case 0b0111: // JAL
    // 0000(op) 000000000000(imm)
    decode_result->imm = (uint16_t)sign_extend(instr & 0b0000111111111111, 12);
    break;
  default:
    break;
  }
}

int execute(struct state *state, struct instruction instr) {
  uint16_t next_pc = state->pc + 1;

  uint16_t opcode = instr.opcode;
  uint16_t rd = instr.rd;
  uint16_t rs1 = instr.rs1;
  uint16_t rs2 = instr.rs2;
  uint16_t func = instr.func;
  uint16_t imm = instr.imm;

  switch (opcode) {
  case 0b0000:
    printf("ALU\n");
    state->regs[rd] = alu(func, state->regs[rs1], state->regs[rs2]);
    break;
  case 0b0001:
    printf("ADDI\n");
    state->regs[rd] = state->regs[rd] + imm;
    break;
  case 0b0010:
    printf("SUBI\n");
    state->regs[rd] = state->regs[rd] - imm;
    break;
  case 0b0011:
    printf("LOAD\n");
    uint16_t load_addr = state->regs[rs1] + imm;
    state->regs[rd] = state->mem[load_addr];
    break;
  case 0b0100:
    printf("STORE\n");
    uint16_t store_addr = state->regs[rs1] + imm;
    if (store_addr < 0x2000) {
      printf("Write to ROM!\n");
      return 1;
    }
    state->mem[store_addr] = state->regs[rd];
    break;
  case 0b0101:
    printf("BEQZ\n");
    if (state->regs[rd] == 0) {
      next_pc += (int16_t)imm;
    }
    break;
  case 0b0110:
    printf("BNEZ\n");
    if (state->regs[rd] != 0) {
      next_pc += (int16_t)imm;
    }
    break;
  case 0b0111:
    printf("JAL\n");
    // set return address
    state->regs[6] = next_pc;
    // jump
    next_pc += imm;
    break;
  case 0b1000:
    printf("RET\n");
    next_pc = state->regs[6];
    break;
  case 0b1001:
    printf("LUI\n");
    state->regs[rd] = imm << 7;
    break;
  case 0b1111:
    printf("HALT\n");
    state->running = false;
    break;
  default:
    printf("undefined\n");
    return 1;
  }

  state->regs[0] = 0;
  state->pc = next_pc;

  return 0;
}

int step(struct state *state) {
  uint16_t instr = state->mem[state->pc];
  struct instruction decode_result;

  decode(instr, &decode_result);
  execute(state, decode_result);

  return 0;
}

void run(struct state *state) {
  while (state->running) {
    if (step(state) != 0) {
      printf("CPU Panic: Exception occurred at PC=0x%04X\n", state->pc);
      break;
    }
  }
}

int main(void) {
  static struct state state;
  init_state(&state);
  state.mem[0] = 0b0000000000000111;
  state.mem[1] = 0b0100000000000000;
  state.mem[1] = 0b1111100000000000;

  run(&state);

  return 0;
}
