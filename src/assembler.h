#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "emulator.h"
#include <stdint.h>
#include <stdbool.h>

enum inst_type { TYPE_R, TYPE_I6, TYPE_I9, TYPE_J, TYPE_BR, TYPE_SP };

struct symbol {
    char *name;
    uint16_t address;
};

struct parsed_line {
    char *operation;
    int regs[3];
    int imm;
    char *target_label;
    char *label_name;
    bool is_label;
};

struct instruction_info {
  char *name;
  uint16_t opcode;
  uint16_t func;
  enum inst_type type;
};

int get_label_address(struct symbol *label_table[], char *label_name, int label_count);
bool register_label(struct symbol *label_table[], char *label_name, int address, int label_count);

struct instruction_info* get_instruction_info(char *name);

int tokenize_line(char *line, char *ret[]);
int parse_line(char *line, struct parsed_line *parse_result);
int gen_code_line(char *line, struct symbol *label_table[], int *label_count, int *current_address);

int gen_code(char *file_name);

#endif
