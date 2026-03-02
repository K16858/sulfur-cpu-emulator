#ifndef CPU_H
#define CPU_H

#include <stdint.h>

struct table {
    char *name;
    uint16_t address;
};

int tokenize_line(char *line, char *ret[]);

#endif
