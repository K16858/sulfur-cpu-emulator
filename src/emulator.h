#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdint.h>
#include <stdbool.h>

#define ROM_END  0x1FFF
#define RAM_START 0x2000
#define RAM_END   0xEFFF
#define IO_START  0xF000

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

#endif
