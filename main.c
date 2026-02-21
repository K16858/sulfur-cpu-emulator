#include <stdint.h>
#include <stdbool.h>

struct state {
    uint16_t pc;
    uint16_t regs[8];
    uint16_t mem[65536];
    bool Z, N, C;
    bool running;
};
