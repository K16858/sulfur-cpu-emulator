#include "utils.h"

int subst(char *str, char c1, char c2) {
  int c = 0;
  while (*str != '\0') {
    if (*str == c1) {
      *str = c2;
      c++;
    }
    str++;
  }
  return c;
}

int split(char *str, char *ret[], char sep, int max) {
  int count = 0;
  char *p = str;

  while (*str != '\0') {
    if (*str == sep && count < max) {
      *str = '\0';
      p = str + 1;
      count++;
    }

    ret[count] = p;
    str++;
  }

  return count;
}
