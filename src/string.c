#include "string.h"

size_t kstrlen(const char *str) {
  size_t len = 0;
  while (str[len])
    len++;
  return len;
}

int kstrcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char *kstrcpy(char *dest, const char *src) {
  char *d = dest;
  while ((*d++ = *src++))
    ;
  return dest;
}

void kitoa(int n, char *str) {
  int i = 0;
  bool is_neg = false;
  if (n == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }
  if (n < 0) {
    is_neg = true;
    n = -n;
  }
  while (n != 0) {
    str[i++] = (n % 10) + '0';
    n /= 10;
  }
  if (is_neg)
    str[i++] = '-';
  str[i] = '\0';

  // Reverse
  int start = 0;
  int end = i - 1;
  while (start < end) {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    start++;
    end--;
  }
}
