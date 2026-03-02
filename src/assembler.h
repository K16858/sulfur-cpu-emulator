#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

struct table {
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

int tokenize_line(char *line, char *ret[]);
int parse_line(char *line, struct parsed_line parse_result);

#endif
