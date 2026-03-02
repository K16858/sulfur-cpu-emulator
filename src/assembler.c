#include "assembler.h"
#include "utils.h"

#define MAX_NAME_LEN 100

int tokenize_line(char *line, char *ret[]) {
  subst(line, ',', ' ');
  return split(line, ret, ' ', 10);
}
