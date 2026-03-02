#include "assembler.h"
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
  return i;
}

// void run(struct state *state) {
//   while (state->running) {
//     if (step(state) != 0) {
//       printf("CPU Panic: Exception occurred at PC=0x%04X\n", state->pc);
//       break;
//     }
//   }
// }

int main(void) {
  // static struct state state;
  // init_state(&state);
  // char *file = "tests/test.mem";

  // int word_count = load_mem(file, state.mem);
  // if (word_count > 0) {
  //   printf("Successfully loaded %d words.\n", word_count);
  // } else {
  //   printf("Failed to load program or file is empty.\n");
  // }

  // run(&state);

  char code[] = "LABEL: addi $r1, $r2, 10000";

  struct parsed_line result;

  parse_line(code, &result);

  printf("operator: %s\n", result.operation);

  for (int i = 0; i < 3; i++) {
    printf("reg%d: %d\n", i, result.regs[i]);
  }

  printf("imm: %d\n", result.imm);
  printf("lable: %s\n", result.target_label);

  return 0;
}
