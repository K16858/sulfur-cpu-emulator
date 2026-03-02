#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdbool.h>

int subst(char *str, char c1, char c2);
int split(char *str, char *ret[], char sep, int max);
bool end_with(char *str, char target);

#endif
