#include "cpu.h"
#include "emulator.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

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
