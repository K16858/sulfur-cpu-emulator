#include "cpu.h"
#include "emulator.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define INST_LEN 6

int load_machinelang(char *file, uint16_t *mem[]) {
  FILE *fp = fopen(file, "r");

  if (fp == NULL) {
    printf("Error: Cannot open file: %s\n", file);
    return 1;
  }

  char buf[INST_LEN + 1];
  char *endp;
  int i = 0;

  while (fgets(buf, INST_LEN, fp)) {
    *mem[i] = (uint16_t)strtol(buf, &endp, 0);
    i++;
  }

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
  state.mem[2] = 0b1111100000000000;

  run(&state);

  return 0;
}
