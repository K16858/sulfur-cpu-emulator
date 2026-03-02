#include "utils.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

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

  while (*str != '\0' && count < max) {
    while (*str == sep && *str != '\0') {
      str++;
    }
    if (*str == '\0')
      break;
    ret[count] = str;
    count++;

    while (*str != sep && *str != '\0') {
      str++;
    }

    if (*str == sep) {
      *str = '\0';
      str++;
    }
  }

  return count;
}

bool end_with(char *str, char target) {
  size_t len = strlen(str);

  if (len == 0) {
    return false;
  }

  if (str[len - 1] == target) {
    return true;
  } else {
    return false;
  }
}
