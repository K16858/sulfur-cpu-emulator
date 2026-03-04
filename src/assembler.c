#include "assembler.h"
#include "string.h"
#include "utils.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NAME_LEN 100
#define MAX_LABEL 1024

int get_label_address(struct symbol *label_table[], char *label_name,
                      int label_count) {
  for (int i = 0; i < label_count; i++) {
    if (strcmp(label_table[i]->name, label_name) == 0) {
      return label_table[i]->address;
    }
  }

  return -1;
}

bool register_lable(struct symbol *label_table[], char *label_name, int address,
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

// int gen_code_line(char *line) {
//   int address = 0;
//   struct parsed_line parse_result;

//   parse_line(line, &parse_result);
//   if (parse_result.is_label) {
//   }
// }
