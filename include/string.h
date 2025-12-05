#ifndef STRING_H
#define STRING_H

#include "types.h"

size_t kstrlen(const char *str);
int kstrcmp(const char *s1, const char *s2);
char *kstrcpy(char *dest, const char *src);
void kitoa(int n, char *str);

#endif
