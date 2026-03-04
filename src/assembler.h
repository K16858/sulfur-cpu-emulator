#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

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

int get_label_address(struct symbol *label_table[], char *label_name, int label_count);
bool register_lable(struct symbol *label_table[], char *label_name, int address, int label_count);

int tokenize_line(char *line, char *ret[]);
int parse_line(char *line, struct parsed_line *parse_result);
int gen_code_line(char *line);

#endif
