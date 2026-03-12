#include "assembler.h"
#include "cpu.h"
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
  return i;
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

  gen_code("tests/forward_ref.s");

  static struct state state;
  init_state(&state);
  char *file = "tests/forward_ref.mem";

  int word_count = load_mem(file, state.mem);
  if (word_count > 0) {
    printf("Successfully loaded %d words.\n", word_count);
  } else {
    printf("Failed to load program or file is empty.\n");
  }

  run(&state);

  return 0;
}
