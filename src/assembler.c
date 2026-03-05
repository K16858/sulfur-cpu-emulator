#include "assembler.h"
#include "string.h"
#include "utils.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LEN 512
#define MAX_NAME_LEN 100
#define MAX_LABEL 1024

struct instruction_info instruction_table[] = {
    {"add", 0b0000, 0b000, TYPE_R}, {"sub", 0b0000, 0b001, TYPE_R},
    {"slt", 0b0000, 0b010, TYPE_R}, {"and", 0b0000, 0b011, TYPE_R},
    {"or", 0b0000, 0b100, TYPE_R},  {"xor", 0b0000, 0b101, TYPE_R},
    {"shl", 0b0000, 0b110, TYPE_R}, {"shr", 0b0000, 0b111, TYPE_R},
    {"addi", 0b0001, 0, TYPE_I9},   {"subi", 0b0010, 0, TYPE_I9},
    {"ld", 0b0011, 0, TYPE_I6},     {"sr", 0b0100, 0, TYPE_I6},
    {"beqz", 0b0101, 0, TYPE_BR},   {"bnez", 0b0110, 0, TYPE_BR},
    {"jal", 0b0111, 0, TYPE_J},     {"ret", 0b1000, 0, TYPE_SP},
    {"halt", 0b1111, 0, TYPE_BR},
};

struct instruction_info *get_instruction_info(char *name) {
  for (int i = 0; i < sizeof(instruction_table) / sizeof(instruction_table[0]);
       i++) {
    if (strcmp(name, instruction_table[i].name) == 0) {
      return &instruction_table[i];
    }
  }

  return NULL;
}

int get_label_address(struct symbol *label_table[], char *label_name,
                      int label_count) {
  for (int i = 0; i < label_count; i++) {
    if (strcmp(label_table[i]->name, label_name) == 0) {
      return label_table[i]->address;
    }
  }

  return -1;
}

bool register_label(struct symbol *label_table[], char *label_name, int address,
                    int label_count) {
  if (get_label_address(label_table, label_name, label_count) != -1) {
    return false;
  }

  if (label_name == NULL) {
    return false;
  }

  if (label_count < MAX_LABEL) {
    label_table[label_count]->name = strdup(label_name);
    label_table[label_count]->address = address;

    return true;
  }

  return false;
}

int tokenize_line(char *line, char *ret[]) {
  subst(line, ',', ' ');
  return split(line, ret, ' ', 10);
}

int parse_line(char *line, struct parsed_line *parse_result) {
  char *ret[10];
  int token_count = tokenize_line(line, ret);

  if (token_count < 1) {
    printf("Error: Tokenize failed!");
    return 1;
  }

  if (end_with(ret[0], ':')) {
    subst(ret[0], ':', '\0');
    parse_result->is_label = true;
    parse_result->label_name = strdup(ret[0]);
    if (token_count == 1) {
      return 0;
    } else {
      parse_result->operation = strdup(ret[1]);
      char *endptr;
      for (int i = 2; i < token_count; i++) {
        if (*ret[i] == '$' && *(ret[i] + 1) == 'r') {
          parse_result->regs[i - 2] = strtol((ret[i] + 2), &endptr, 10);
        } else if (isalpha((unsigned char)*ret[i])) {
          parse_result->target_label = strdup(ret[i]);
          parse_result->regs[i - 2] = -1;
        } else {
          parse_result->imm = strtol(ret[i], &endptr, 10);
          parse_result->regs[i - 2] = -1;
        }
      }

      return 0;
    }
  } else {
    parse_result->is_label = false;
    parse_result->operation = strdup(ret[0]);

    char *endptr;
    for (int i = 1; i < token_count; i++) {
      if (*ret[i] == '$' && *(ret[i] + 1) == 'r') {
        parse_result->regs[i - 1] = strtol((ret[i] + 2), &endptr, 10);
      } else if (isalpha((unsigned char)*ret[i])) {
        parse_result->target_label = strdup(ret[i]);
        parse_result->regs[i - 1] = -1;
      } else {
        parse_result->imm = strtol(ret[i], &endptr, 10);
        parse_result->regs[i - 1] = -1;
      }
    }

    return 0;
  }
}

int gen_code_line(char *line, struct symbol *label_table[], int *label_count,
                  int *current_address) {
  struct parsed_line parse_result = {0};
  struct instruction_info *instr_info;

  parse_line(line, &parse_result);
  if (parse_result.is_label) {
    if (!register_label(label_table, parse_result.label_name, *current_address,
                        *label_count)) {
      printf("Failed to register label: %s\n", parse_result.label_name);
    } else {
      (*label_count)++;
    }
  }


  if (parse_result.operation == NULL) {
    if (parse_result.label_name != NULL) free(parse_result.label_name);
    if (parse_result.target_label != NULL) free(parse_result.target_label);
    return 0;
  }

  instr_info = get_instruction_info(parse_result.operation);
  
  if (instr_info == NULL) {
    fprintf(stderr, "Error: Unknown instruction '%s'\n", parse_result.operation);
    if (parse_result.label_name != NULL) free(parse_result.label_name);
    if (parse_result.operation != NULL) free(parse_result.operation);
    if (parse_result.target_label != NULL) free(parse_result.target_label);
    return 1;
  }

  uint16_t opcode = instr_info->opcode;
  uint16_t reg0 = (uint16_t)parse_result.regs[0];
  uint16_t reg1 = (uint16_t)parse_result.regs[1];
  uint16_t reg2 = (uint16_t)parse_result.regs[2];
  uint16_t imm = (uint16_t)parse_result.imm;

  if (parse_result.target_label != NULL) {
    int target_address = get_label_address(label_table, parse_result.target_label, *label_count);
    if (target_address == -1) {
      fprintf(stderr, "Error: Undefined label '%s'\n", parse_result.target_label);
      if (parse_result.label_name != NULL) free(parse_result.label_name);
      if (parse_result.operation != NULL) free(parse_result.operation);
      if (parse_result.target_label != NULL) free(parse_result.target_label);
      return 1;
    }

    int offset = target_address - (*current_address + 1);
    imm = (uint16_t)offset;
  }

  uint16_t instr;

  switch (instr_info->type) {
  case TYPE_R:
    instr = (uint16_t)((opcode << 12) | (reg0 << 9) | (reg1 << 6) |
                       (reg2 << 3) | instr_info->func);
    printf("0x%04X\n", instr);
    break;
  case TYPE_I6:
    instr = (uint16_t)((opcode << 12) | (reg0 << 9) | (reg1 << 6) | imm);
    printf("0x%04X\n", instr);
    break;
  case TYPE_I9:
    instr = (uint16_t)((opcode << 12) | (reg0 << 9) | imm);
    printf("0x%04X\n", instr);
    break;
  case TYPE_BR:
    instr = (uint16_t)((opcode << 12) | (reg0 << 9) | imm);
    printf("0x%04X\n", instr);
    break;
  case TYPE_J:
    instr = (uint16_t)((opcode << 12) | imm);
    printf("0x%04X\n", instr);
    break;
  case TYPE_SP:
    instr = (uint16_t)((opcode << 12));
    printf("0x%04X\n", instr);
    break;
  default:
    break;
  }

  (*current_address)++;
  
  if (parse_result.label_name != NULL) free(parse_result.label_name);
  if (parse_result.operation != NULL) free(parse_result.operation);
  if (parse_result.target_label != NULL) free(parse_result.target_label);
  
  return 0;
}

int gen_code(char *file) {
  FILE *fp = fopen(file, "r");

  if (fp == NULL) {
    printf("Error: Cannot open file: %s\n", file);
    return 0;
  }

  struct symbol *label_table[MAX_LABEL];
  
  for (int i = 0; i < MAX_LABEL; i++) {
    label_table[i] = (struct symbol*)malloc(sizeof(struct symbol));
    if (label_table[i] == NULL) {
      fprintf(stderr, "Error: Memory allocation failed\n");
      for (int j = 0; j < i; j++) {
        if (label_table[j]->name != NULL) free(label_table[j]->name);
        free(label_table[j]);
      }
      fclose(fp);
      return -1;
    }
    label_table[i]->name = NULL;
    label_table[i]->address = 0;
  }

  char line[MAX_LINE_LEN + 1];
  int i = 0;
  int label_count = 0;
  int current_address = 0;

  while (fgets(line, MAX_LINE_LEN, fp)) {
    subst(line, '\n', '\0');
    subst(line, '\r', '\0');
    subst(line, '\t', ' ');
    subst(line, '#', '\0');

    if (line[0] == '\0') {
      continue;
    }
    gen_code_line(line, label_table, &label_count, &current_address);
    i++;
  }

  fclose(fp);

  for (int i = 0; i < MAX_LABEL; i++) {
    if (label_table[i]->name != NULL) {
      free(label_table[i]->name);
    }
    free(label_table[i]);
  }

  return i;
}
