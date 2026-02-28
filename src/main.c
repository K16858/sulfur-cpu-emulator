#include "cpu.h"
#include "emulator.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define MAX_LINE_LEN 512

int load_mem(char *file, uint16_t *mem) {
  FILE *fp = fopen(file, "r");

  if (fp == NULL) {
    printf("Error: Cannot open file: %s\n", file);
    return 1;
  }

  char buf[MAX_LINE_LEN + 1];
  char *endp;
  int i = 0;

  while (fgets(buf, MAX_LINE_LEN, fp) && i < 65536) {
    if (buf[0] == '\n' || buf[0] == '#') {
      continue;
    }
    mem[i] = (uint16_t)strtol(buf, &endp, 0);
    i++;
  }

  fclose(fp);
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
  char *file = "tests/test.mem";

  load_mem(file, state.mem);

  run(&state);

  return 0;
}
